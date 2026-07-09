%global majver 1
Name:          lingmo-icon
Version:       1.0.0
Release:       1%{?dist}
Summary:       Lingmo Desktop - Icon theme engine
License:       GPL-2.0-or-later
URL:           https://lingmo.org
Source0:       %{name}-%{version}.tar.gz
BuildRequires: cmake >= 3.21 ninja-build qt6-qtbase-devel qt6-qtsvg-devel
%description
Provides XDG icon theme loading with fallback chain and SVG support.
%package libs
Summary: Shared library for %{name}
%description libs
Shared library for Lingmo icon theme engine.
%package devel
Summary: Development files for %{name}
Requires: %{name}-libs%{?_isa} = %{version}-%{release} qt6-qtbase-devel
%description devel
Development headers and CMake integration for Lingmo Icon.
%prep
%setup -q
%build
%cmake -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBUILD_TESTING=OFF
%cmake_build
%install
%cmake_install
%files libs
%license LICENSE
%{_libdir}/libLingmoIcon.so.%{majver}*
%files devel
%{_includedir}/LingmoIcon/
%{_libdir}/libLingmoIcon.so
%{_libdir}/cmake/LingmoIcon/
%changelog
* Wed Jul 09 2026 Lingmo OS Team <team@lingmo.org> - 1.0.0-1
- Initial release
