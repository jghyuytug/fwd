{
    if (/^[a-zA-Z_].*::/ && !/std::/ && !/__gnu_cxx::/ && !/^\/\//) {
        print "// " $0 " // [Member]"
    } else {
        print $0
    }
}
