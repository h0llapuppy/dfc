Name: dfc
Version: 3.1.1
Release: alt1

Summary: Report file system space usage information with style
License: BSD-3-Clause
Group: Monitoring

URL: https://github.com/rolinh/dfc
Source: %name-%version.tar

BuildRequires: cmake 
BuildRequires: gcc
BuildRequires: gettext

%description
dfc is a tool to report file system space usage information. When the
output is a terminal, it uses color and graphs by default. It has a lot of
features such as HTML, JSON and CSV export, multiple filtering options,
the ability to show mount options and so on.

%prep
%setup

%build
%cmake -DSYSCONFDIR=%_sysconfdir 
%cmake_build

%install
%cmake_install

rm -f %buildroot%_defaultdocdir/%name/HACKING.md

%find_lang %name
%find_lang --with-man %name
 
%files -f %name.lang
%doc LICENSE AUTHORS.md CHANGELOG.md README.md TRANSLATORS.md
%_bindir/%name
%_mandir/man1//%name.1*
%config(noreplace) %_sysconfdir/xdg/%name/

%changelog
* Fri Sep 30 2022 Dmitriy Golovin <hollapuppy@altlinux.org> 3.1.1-alt1
- 3.1.1

