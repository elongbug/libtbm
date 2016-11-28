%bcond_with x
%bcond_with wayland
%bcond_with utest

Name:           libtbm
Version:        2.0.9
Release:        1
License:        MIT
Summary:        The library for Tizen Buffer Manager
Group:          System/Libraries
Source0:        %{name}-%{version}.tar.gz
Source1001:		%name.manifest

BuildRequires:  pkgconfig(libdrm)
BuildRequires:  pkgconfig(wayland-server)
BuildRequires:  pkgconfig(wayland-client)
BuildRequires:  pkgconfig(capi-base-common)
BuildRequires:  pkgconfig(libpng)
BuildRequires:  pkgconfig(dlog)

%if %{with utest}
BuildRequires:  gtest-devel
%endif

%description
Description: %{summary}

%package devel
Summary:        Tizen Buffer Manager Library - Development
Group:          Development/Libraries
Requires:       libtbm = %{version}
Requires:       pkgconfig(capi-base-common)

%description devel
The library for Tizen Buffer Manager.

Development Files.

%global TZ_SYS_RO_SHARE  %{?TZ_SYS_RO_SHARE:%TZ_SYS_RO_SHARE}%{!?TZ_SYS_RO_SHARE:/usr/share}

%prep
%setup -q
cp %{SOURCE1001} .

%build
UTEST="no"

%if %{with utest}
UTEST="yes"
%endif

%if %{with wayland}
%reconfigure --prefix=%{_prefix} --with-tbm-platform=WAYLAND  --with-utest=${UTEST} \
            CFLAGS="${CFLAGS} -Wall -Werror" LDFLAGS="${LDFLAGS} -Wl,--hash-style=both -Wl,--as-needed"
%else
%reconfigure --prefix=%{_prefix} --with-tbm-platform=X11  --with-utest=${UTEST} \
            CFLAGS="${CFLAGS} -Wall -Werror" LDFLAGS="${LDFLAGS} -Wl,--hash-style=both -Wl,--as-needed"
%endif

make %{?_smp_mflags}

%if %{with utest}
make -C ut check
%endif

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/%{TZ_SYS_RO_SHARE}/license
cp -af COPYING %{buildroot}/%{TZ_SYS_RO_SHARE}/license/%{name}
%make_install


%__mkdir_p %{buildroot}%{_unitdir}
install -m 644 service/tbm-drm-auth.service %{buildroot}%{_unitdir}
install -m 644 service/tbm-drm-auth.path %{buildroot}%{_unitdir}
%__mkdir_p %{buildroot}%{_unitdir_user}
install -m 644 service/tbm-drm-auth-user.service %{buildroot}%{_unitdir_user}
install -m 644 service/tbm-drm-auth-user.path %{buildroot}%{_unitdir_user}

%clean
rm -rf %{buildroot}

%pre
%__mkdir_p %{_unitdir}/graphical.target.wants
ln -sf ../tbm-drm-auth.path %{_unitdir}/graphical.target.wants/

%__mkdir_p %{_unitdir_user}/basic.target.wants
ln -sf ../tbm-drm-auth-user.path %{_unitdir_user}/basic.target.wants/

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig
rm -f %{_unitdir}/graphical.target.wants/tbm-drm-auth.path

rm -f %{_unitdir_user}/basic.target.wants/tbm-drm-auth-user.path

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{TZ_SYS_RO_SHARE}/license/%{name}
%{_libdir}/libtbm.so.*
%{_unitdir}/tbm-drm-auth.path
%{_unitdir}/tbm-drm-auth.service
%{_unitdir_user}/tbm-drm-auth-user.path
%{_unitdir_user}/tbm-drm-auth-user.service
%if %{with utest}
%{_bindir}/ut
%endif

%files devel
%manifest %{name}.manifest
%defattr(-,root,root,-)
%dir %{_includedir}
%{_includedir}/tbm_bufmgr.h
%{_includedir}/tbm_surface.h
%{_includedir}/tbm_surface_internal.h
%{_includedir}/tbm_surface_queue.h
%{_includedir}/tbm_bufmgr_backend.h
%{_includedir}/tbm_type.h
%{_includedir}/tbm_drm_helper.h
%{_includedir}/tbm_sync.h
%{_libdir}/libtbm.so
%{_libdir}/pkgconfig/libtbm.pc
