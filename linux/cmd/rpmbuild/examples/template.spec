# cat template.spec
Name: test
Version:	1.0.0
Release:        1%{?dist}
Summary:	Test

License:	GPL-2.0
URL:	https://github.com/rtoax/
Source0:	main.c

BuildRequires: bash
Requires:	bash

%description
This is %{name}

%prep
%autosetup


%build
%configure
%make_build


%install
rm -rf $RPM_BUILD_ROOT
%make_install


%files
%license LICENSE
%doc README.md



%changelog
* Sat Feb 05 2022 RongTao
