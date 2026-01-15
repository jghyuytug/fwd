FROM centos:7

# Fix CentOS repository configuration - use Vault for reliable access
RUN sed -i 's/mirrorlist/#mirrorlist/g' /etc/yum.repos.d/CentOS-*.repo && \
    sed -i 's|#baseurl=http://mirror.centos.org|baseurl=https://vault.centos.org|g' /etc/yum.repos.d/CentOS-*.repo

# Install MySQL official repository
RUN yum install -y https://dev.mysql.com/get/mysql80-community-release-el7-1.noarch.rpm

# Install build tools and 32-bit libraries
RUN yum install -y --nogpgcheck \
    glibc-devel.i686 \
    libstdc++-devel.i686 \
    mysql-community-devel.i686 \
    GeoIP-devel.i686 \
    make \
    gcc \
    gcc-c++ \
    git \
    which \
    wget \
    && yum clean all

# Set working directory
WORKDIR /build

# Default command
CMD ["/bin/bash"]
