Summary: A fast, lightweight, cross-platform text editor
Name: dred
Version: 0.3
Release: b1
Source0: %{name}-%{version}.tar.bz2
License: GPL
Group: Applications/System
BuildRoot: %{_tmppath}/%{name}-root

%description
A fast, lightweight, cross-platform text editor.

%prep
%setup -q

%build
clang -Os source/dred/dred_main.c -o dred `pkg-config --cflags --libs gtk+-3.0` -lm -ldl

%install
mkdir -p $RPM_BUILD_ROOT/{%{_bindir},%{_datadir}/applications}
cp dred $RPM_BUILD_ROOT/%{_bindir}
cp -a resources/unix/dred.desktop $RPM_BUILD_ROOT/%{_datadir}/applications/

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%attr(755,root,root) %{_bindir}/dred
%{_datadir}/applications/*.desktop


%changelog
* Fri Nov 24 2017 Yuichiro Nakada <berry@berry-lab.net>
- Create for Berry Linux
