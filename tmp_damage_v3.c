
/*==============================================================================
 * Phase 10: Enhanced Damage Calculation System
 *============================================================================*/

/* Damage modifier structure - collects all modifiers before final calculation */
typedef struct {
    int32_t attack_percent;      /* Attack power bonus % (from buffs, equipment) */
    int32_t damage_percent;      /* Final damage bonus % */
    int32_t crit_damage_bonus;   /* Critical damage multiplier bonus (150 base + this) */
    int32_t def_penetration;     /* Fixed defense penetration */
    int32_t def_penetration_pct; /* % defense penetration */
    int32_t elem_bonus[6];       /* Elemental damage bonus % */
    uint8_t guaranteed_crit;     /* Force critical hit */
    uint8_t ignore_defense;      /* Ignore all defense */
} DamageModifiers;

/* Collect all damage modifiers from session (buffs, equipment, etc) */
static void collect_damage_modifiers(ClientSession* s, DamageModifiers* mods) {
    memset(mods, 0, sizeof(DamageModifiers));
    if (!s) return;

    /* Buff modifiers */
    mods->attack_percent += buff_get_stat_modifier(s, BUFF_ATTACK_UP);
    mods->attack_percent += buff_get_stat_modifier(s, BUFF_MAGIC_UP);
    mods->attack_percent += buff_get_stat_modifier(s, BUFF_ALL_STATS_UP);

    /* Damage increase/decrease buffs */
    mods->damage_percent += buff_get_stat_modifier(s, BUFF_DAMAGE_UP);

    /* Critical damage from buffs (base is 150%, additional from buffs) */
    mods->crit_damage_bonus = 0;

    /* Debuff effects on player (reduce damage) */
    int32_t curse = buff_get_stat_modifier(s, DEBUFF_CURSE);
    if (curse > 0) {
        mods->damage_percent -= curse;
    }

    /* Berserk mode */
    if (buff_has(s, BUFF_BERSERK)) {
        mods->damage_percent += 30;
        mods->crit_damage_bonus += 20;
    }
}

/* Enhanced damage calculation with all modifiers */
static uint32_t calculate_skill_damage_v3(ClientSession* s, uint16_t skill_id,
                                          uint8_t skill_level, uint16_t monster_type,
                                          uint8_t monster_level, uint32_t monster_def,
                                          uint8_t* is_crit, uint8_t* is_miss) {
    if (!s) return 0;

    CombatStats* stats = &s->combat_stats;
    uint8_t job = s->job;
    uint8_t char_level = s->level;

    if (skill_level < 1) skill_level = 1;
    if (skill_level > 30) skill_level = 30;
    if (is_crit) *is_crit = 0;
    if (is_miss) *is_miss = 0;

    /* Collect all modifiers */
    DamageModifiers mods;
    collect_damage_modifiers(s, &mods);

    /* Find skill info */
    const SkillDamageInfo* skill = find_skill_info(skill_id);
    if (!skill) {
        uint32_t base = stats->phys_attack / 2;
        return base > monster_def ? base - monster_def : 1;
    }

    /* Miss check (hit rate vs evasion) */
    uint32_t hit_chance = stats->hit_rate;
    if (monster_level > char_level) {
        int level_diff = monster_level - char_level;
        hit_chance = hit_chance * (100 - level_diff * 2) / 100;
    }
    uint32_t miss_roll = (uint32_t)rand() % 10000;
    if (miss_roll >= hit_chance) {
        if (is_miss) *is_miss = 1;
        printf("[COMBAT] Attack MISSED! hit_chance=%u, roll=%u\n", hit_chance, miss_roll);
        return 0;
    }

    /* Get monster resistance */
    const MonsterResist* resist = find_monster_resist(monster_type);

    /* Get base attack power */
    uint32_t attack_power;
    uint32_t defense = monster_def;
    uint8_t resist_percent = 0;

    switch (skill->type) {
        case SKILL_TYPE_MAGICAL:
            attack_power = stats->mag_attack;
            resist_percent = resist ? resist->mag_resist : 0;
            break;
        case SKILL_TYPE_HYBRID:
            attack_power = (stats->phys_attack > stats->mag_attack) ?
                           stats->phys_attack : stats->mag_attack;
            defense = monster_def / 2;
            resist_percent = resist ? (resist->phys_resist + resist->mag_resist) / 4 : 0;
            break;
        case SKILL_TYPE_FIXED:
            attack_power = 0;
            defense = 0;
            resist_percent = 0;
            break;
        case SKILL_TYPE_PHYSICAL:
        default:
            attack_power = stats->phys_attack;
            resist_percent = resist ? resist->phys_resist : 0;
            break;
    }

    /* Apply attack power modifier from buffs */
    if (mods.attack_percent != 0) {
        attack_power = attack_power * (100 + mods.attack_percent) / 100;
    }

    /* Calculate base skill damage */
    uint32_t skill_base = skill->base_damage + (skill->damage_per_level * (skill_level - 1));
    uint32_t attack_bonus = (attack_power * skill->attack_percent) / 100;
    uint32_t total_damage = skill_base + attack_bonus;

    /* Apply level modifier */
    uint32_t level_mod = calculate_level_modifier(char_level, monster_level);
    total_damage = (total_damage * level_mod) / 100;

    /* Apply defense penetration */
    if (mods.ignore_defense) {
        defense = 0;
    } else {
        if (mods.def_penetration > 0 && defense > (uint32_t)mods.def_penetration) {
            defense -= mods.def_penetration;
        }
        if (mods.def_penetration_pct > 0) {
            defense = defense * (100 - mods.def_penetration_pct) / 100;
        }
    }

    /* Apply defense reduction */
    uint32_t damage_after_def;
    if (total_damage > defense) {
        damage_after_def = total_damage - defense;
    } else {
        damage_after_def = total_damage / 10;
        if (damage_after_def < 1) damage_after_def = 1;
    }

    /* Apply type resistance */
    if (resist_percent > 0) {
        damage_after_def = damage_after_def * (100 - resist_percent) / 100;
    }

    /* Apply elemental bonus/resistance */
    if (skill->element != ELEMENT_NONE && skill->element < 6) {
        int32_t elem_bonus = mods.elem_bonus[skill->element];
        if (elem_bonus > 0) {
            damage_after_def = damage_after_def * (100 + elem_bonus) / 100;
        }
        if (resist) {
            uint8_t elem_resist = get_element_resist(resist, skill->element);
            if (elem_resist > 0) {
                damage_after_def = damage_after_def * (100 - elem_resist) / 100;
            }
        }
    }

    /* Critical hit check */
    uint8_t crit = 0;
    uint32_t crit_rate = stats->crit_rate;
    if (mods.guaranteed_crit) {
        crit = 1;
    } else {
        uint32_t rand_val = (uint32_t)rand() % 10000;
        if (rand_val < crit_rate) {
            crit = 1;
        }
    }

    if (crit) {
        uint32_t crit_multiplier = 150 + mods.crit_damage_bonus;
        damage_after_def = damage_after_def * crit_multiplier / 100;
        printf("[COMBAT] CRITICAL HIT! Multiplier: %u%%\n", crit_multiplier);
    }
    if (is_crit) *is_crit = crit;

    /* Apply final damage modifier */
    if (mods.damage_percent != 0) {
        int32_t modified = (int32_t)damage_after_def * (100 + mods.damage_percent) / 100;
        damage_after_def = (modified > 0) ? (uint32_t)modified : 1;
    }

    /* Add variance (95%-105%) */
    uint32_t variance = (uint32_t)rand() % 11;
    damage_after_def = damage_after_def * (95 + variance) / 100;

    if (damage_after_def < 1) damage_after_def = 1;

    return damage_after_def;
}

/* Apply lifesteal effect after dealing damage */
static void apply_lifesteal(ClientSession* s, uint32_t damage_dealt) {
    if (!s) return;

    int32_t lifesteal_pct = buff_get_stat_modifier(s, BUFF_LIFESTEAL);
    if (lifesteal_pct > 0 && damage_dealt > 0) {
        uint32_t heal = damage_dealt * lifesteal_pct / 100;
        if (heal > 0) {
            s->current_hp += heal;
            if (s->current_hp > s->combat_stats.max_hp) {
                s->current_hp = s->combat_stats.max_hp;
            }
            printf("[COMBAT] Lifesteal: +%u HP\n", heal);
        }
    }
}

