Summary: standards-compliant multimedia streaming client
Name: nemesi
Version: 0.5.0
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
Requires: libtool-libs >= 1.4
Requires: libtool >= 1.4

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
./configure --enable-optimize=full --prefix=%{_prefix} --mandir=%{_mandir}
make -s

%install
rm -rf %{buildroot}
make -s DESTDIR=%{buildroot} install
rm -rf %{buildroot}%{_datadir}/doc/%{name}

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root)
%{_bindir}/%{name}
%{_bindir}/g%{name}
%{_mandir}/man1/*

%{_libdir}/%{name}/plugins/libffmp3.*
%{_libdir}/%{name}/plugins/gsm_amr_float.*
%{_libdir}/%{name}/plugins/L16_8k_m.*
%{_libdir}/%{name}/plugins/libffmpeg.*

%{_datadir}/%{name}/throbber/*.png
%{_datadir}/%{name}/pixmaps/*.png


%doc README TODO COPYING ChangeLog docs/%{name}.1.*
