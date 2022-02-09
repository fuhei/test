Name: test
Version: 1.0.0
Release:        1%{?dist}
Summary: test

License: GPL-2.0

%description
This is a %{name}.

%prep
echo "prep"
%generate_buildrequires

%build

%install

%check

%clean
