---
**Document Type**: Configuration Directory Navigation
**Generated**: 2025-11-05
**Updated**: 2025-11-05
**Version**: v1.0
**Status**: Complete
---

# 📂 config/ - Configuration Management Directory

## Overview

The `config/` directory contains all project configuration files, including Docker deployment configurations.

**Current Subdirectories**: 1 main directory (docker/)
**Total Configuration Files**: 6 files

---

## 📋 Quick Navigation

### 📍 Subdirectories
- **docker/** - Docker build and deployment configurations

---

## 📂 Directory Structure

```
config/
└── 📂 docker/                 # Docker Configurations (6 files)
    ├── Dockerfile            # Main production container
    ├── Dockerfile.integration # Integration testing container
    ├── Dockerfile.network    # Network layer testing
    ├── Dockerfile.network_extract
    ├── docker-compose.yml    # Compose orchestration
    └── .dockerignore         # Build context exclusions
```

---

## 🐳 docker/ - Docker Configuration Subdirectory

### Purpose
Centralized Docker configuration for containerized builds and deployments.

### Files Inventory

| File | Size | Purpose |
|------|------|---------|
| Dockerfile | 1.8 KB | Main production container |
| Dockerfile.integration | 1.5 KB | Integration testing |
| Dockerfile.network | 3.0 KB | Network layer testing |
| Dockerfile.network_extract | 1.3 KB | Network extraction |
| docker-compose.yml | 417 B | Docker Compose config |
| .dockerignore | 398 B | Build exclusions |

### Quick Reference

**For Building**: Use `Dockerfile`
```bash
docker build -f config/docker/Dockerfile -t df_game_r:latest .
```

**For Testing**: Use `Dockerfile.integration`
```bash
docker build -f config/docker/Dockerfile.integration -t df_game_test:latest .
```

**For Network Work**: Use `Dockerfile.network`
```bash
docker build -f config/docker/Dockerfile.network -t df_game_network:latest .
```

**For Compose**: Use `docker-compose.yml`
```bash
docker-compose -f config/docker/docker-compose.yml up -d
```

### File Details

#### Dockerfile (1.8 KB)
- **Type**: Production container
- **Base**: CentOS 5 (legacy system)
- **Purpose**: Main build environment for df_game_r
- **Use Case**: Full compilation and binary generation

#### Dockerfile.integration (1.5 KB)
- **Type**: Integration testing container
- **Purpose**: Test integration between components
- **Use Case**: Validate component interactions
- **Related**: Integration test suites

#### Dockerfile.network (3.0 KB)
- **Type**: Network testing container
- **Purpose**: Test network layer components
- **Size**: 3.0 KB (largest, indicates more dependencies)
- **Use Case**: Network protocol validation, packet handling tests

#### Dockerfile.network_extract (1.3 KB)
- **Type**: Network extraction container
- **Purpose**: Extract and build network components
- **Use Case**: Modular network layer development

#### docker-compose.yml (417 B)
- **Type**: Docker Compose orchestration
- **Purpose**: Multi-container orchestration configuration
- **Supports**: Running multiple containers together
- **Use Case**: Complete environment setup

#### .dockerignore (398 B)
- **Type**: Docker build context exclusions
- **Purpose**: Reduce build context size
- **Impact**: Faster builds, smaller transfers
- **Contains**: Exclusion patterns

---

## 🎯 Usage Scenarios

### Scenario 1: Full Production Build
```bash
cd config/docker
docker build -f Dockerfile -t df_game_r:prod .
```

### Scenario 2: Quick Compose Deployment
```bash
docker-compose -f config/docker/docker-compose.yml up -d
```

### Scenario 3: Integration Testing
```bash
docker build -f config/docker/Dockerfile.integration -t test:latest .
docker run test:latest
```

### Scenario 4: Network Component Testing
```bash
docker build -f config/docker/Dockerfile.network -t network:latest .
docker run network:latest
```

---

## 📊 Configuration Statistics

| Category | Count |
|----------|-------|
| Total Config Files | 6 |
| Dockerfile variants | 4 |
| Compose configs | 1 |
| Ignore patterns | 1 |
| Total size | ~9 KB |

---

## ✅ Configuration Verification

### Before Using Docker Configs

- [ ] Docker is installed and running
- [ ] Docker daemon is accessible
- [ ] Adequate disk space for images
- [ ] Network access available (for base images)
- [ ] Correct Dockerfile selected for task

### During Docker Build

- [ ] Build context size is acceptable
- [ ] No build errors in logs
- [ ] Image builds successfully
- [ ] Tags are correctly applied

### After Docker Build

- [ ] Verify image created: `docker images`
- [ ] Check image size is reasonable
- [ ] Test container starts: `docker run`
- [ ] Verify all needed tools are in container

---

## 🚀 Quick Start

### Build Main Container
```bash
docker build -f config/docker/Dockerfile -t df_game_r:latest .
docker run df_game_r:latest
```

### Use Docker Compose
```bash
docker-compose -f config/docker/docker-compose.yml up
```

### Clean Up
```bash
docker-compose -f config/docker/docker-compose.yml down
docker rmi df_game_r:latest
```

---

## 📝 Configuration Management

### Best Practices
1. **Keep Dockerfiles in config/docker/** - Single location for all Docker configs
2. **Use descriptive names** - Clear purpose from filename
3. **Version tags** - Tag images with versions
4. **Test before production** - Use integration Dockerfile first
5. **Document changes** - Note any modifications to Dockerfiles

### Common Tasks

**Add new Docker config**:
```bash
cp config/docker/Dockerfile config/docker/Dockerfile.newfeature
# Edit as needed
```

**Build all Docker images**:
```bash
for dockerfile in config/docker/Dockerfile*; do
  docker build -f "$dockerfile" -t df_game_r:$(basename $dockerfile) .
done
```

**Review Docker settings**:
```bash
cat config/docker/docker-compose.yml
```

---

## 🔗 Related Documentation

- **README.md** - Docker build instructions reference config/
- **CLAUDE.md** - Compilation guidelines mention Docker
- **docs/guides/** - Detailed Docker usage guides
- **docs/design/** - Docker architecture decisions

---

## 📞 Troubleshooting

**Docker build fails**:
- Check .dockerignore for excluded files
- Verify base image availability
- Check for syntax errors in Dockerfile

**Container doesn't start**:
- Review Dockerfile CMD/ENTRYPOINT
- Check port bindings in docker-compose.yml
- Verify volume mounts

**Slow builds**:
- Optimize .dockerignore to reduce context
- Use multi-stage Dockerfile if possible
- Cache layers efficiently

---

**Directory Type**: Configuration Management
**Organization**: By container type/purpose
**Total Files**: 6
**Last Reviewed**: 2025-11-05
**Status**: Complete

**Next Steps**: Use docker-compose.yml for quick deployment or individual Dockerfiles for specific tasks.
