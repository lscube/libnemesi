Summary: A NEtwork MEdia Streamer
Name: nemesi
Version: 0.2.99
Release: 0
Copyright: GPL
Group: Applications/Internet
Vendor: OMSP Team (http://streaming.polito.it)
Packager: OMSP Team (http://streaming.polito.it)
Source: http://streaming.polito.it/download/%{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-buildroot
Requires: SDL >= 1.2.2
BuildRequires: SDL-devel >= 1.2.2
Requires: ffmpeg >= 0.4.6
BuildRequires: ffmpeg-devel >= 0.4.6
Requires: libtool-libs >= 1.5
Requires: libtool >= 1.5

%description

NeMeSI -- NEtwork MEdia Streamer I is a streaming client that allows user to
enjoy playback of multimedia files with low latency transport over IP-based
networks. This software is fully compliant with all IETF's protocols for
streaming data that requires real-time class delivery. 

NeMeSI features an adaptive behaviour achieved by the innovative "Dynamic CoDec
Switching" technique.

NeMeSI is the official streaming client/player of Open Media Streaming Project
by Internet Multimedia Group at Politecnico di Torino. Please visite OMSP home
page (http://streaming.polito.it) for further information.


%prep
%setup -q -n %{name}-%{version}

%build
./configure --enable-optimize=full --prefix=/usr
make

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}%{_bindir}
install -m 755 src/%{name} %{buildroot}%{_bindir}
mkdir -p %{buildroot}%{_datadir}/%{name}/plugins
install -m 755 plugins/plugins/*.so %{buildroot}%{_datadir}/%{name}/plugins
install -m 755 plugins/plugins/*.la %{buildroot}%{_datadir}/%{name}/plugins
install -m 755 plugins/plugins/*.a %{buildroot}%{_datadir}/%{name}/plugins

mkdir -p %{buildroot}%{_mandir}/man1
install -m 755 docs/%{name}.1 %{buildroot}%{_mandir}/man1

mkdir -p %{buildroot}%{_datadir}/%{name}/throbber
install -m 644 egui/throbber/*.png %{buildroot}%{_datadir}/%{name}/throbber

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root)
%{_bindir}/%{name}
%{_mandir}/man1/%{name}.1*

%{_datadir}/%{name}/plugins/libffmp3.*
%{_datadir}/%{name}/plugins/gsm_amr_float.*
%{_datadir}/%{name}/plugins/L16_8k_m.*
%{_datadir}/%{name}/plugins/libffmpeg.*

%{_datadir}/%{name}/throbber/*.png

%doc README TODO COPYING ChangeLog docs/%{name}.1.*
