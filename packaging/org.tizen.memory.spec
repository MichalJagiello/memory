%define AppInstallPath /usr/apps/%{name}
%define Exec memory

Name:       org.tizen.memory
Summary:    Empty app
Version:    0.0.1
Release:    0
License:    Apache-2.0
Group: 	    tizen/Application
Source0:    %{name}-%{version}.tar.gz

BuildRequires:  pkgconfig(capi-appfw-application)
BuildRequires:  pkgconfig(appcore-efl)
BuildRequires:  pkgconfig(elementary)
BuildRequires:  pkgconfig(eet)
BuildRequires:  pkgconfig(edje)
BuildRequires:  cmake
BuildRequires:  libopenssl
BuildRequires:  pkgconfig(efl-assist)
%description
Shortcut sample client

%prep
%setup -q

%build
cmake . -DCMAKE_INSTALL_PREFIX="%{AppInstallPath}" -DCMAKE_TARGET="%{Exec}" -DCMAKE_PACKAGE="%{name}"
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install
#install -m666 %{AppInstallPath}/res/results.eet /var/results.eet

%files
%{AppInstallPath}/bin/memory
%{AppInstallPath}/res/icons/*.png
%{AppInstallPath}/res/*.edj
%{AppInstallPath}/res/results.eet
%attr(666, app, app) %{AppInstallPath}/res/results.eet
%{AppInstallPath}/res/images/*.png
%{AppInstallPath}/res/images/*.jpg
/opt/share/packages/org.tizen.memory.xml
