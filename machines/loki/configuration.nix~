# Edit this configuration file to define what should be installed on
# your system.  Help is available in the configuration.nix(5) man page
# and in the NixOS manual (accessible by running ‘nixos-help’).

{ config, pkgs,lib, ... }:
let
wavetal = (with pkgs; stdenv.mkDerivation{
                  pname = "wavetal";
                  version = "1.0.6";
                  src = fetchGit{
                       url = "https://github.com/tank-trax/vital";
                      ref = "vitality+minus-1.0.6";
                      rev = "78e67b0c67552b7e94e17d36bd0873a2c1bae0f3";
                       };
                  nativeBuildInputs = [ pkg-config

                   alsaLib
                   libGL
                   csound
                   glib
                   libjack2
                   libsecret
                   xorg.libXcursor
                   xorg.libX11.dev
                   xorg.libXext
                   xorg.libX11
                   xorg.libX11.out
                   xorg.libXinerama
                   freetype
                   curl
                   ftgl
 ];
                  BuildInputs = [
                   alsaLib
                   libGL
                   csound
                   glib
                   libjack2
                   libsecret
                   xorg.libXcursor
                   xorg.libX11.dev
                   xorg.libXext
                   xorg.libX11
                   xorg.libX11.out
                   xorg.libXinerama
                   cmakeCurses
                   freetype
                   curl
                   ftgl
                 ];
                buildPhase = "make";
                installPhase = ''
                 ls plugin/builds/
                 mkdir -p $out/bin
                 mkdir -p $out/vst3
                 cp plugin/builds/linux_vst/build/Vitality+Minus $out/bin/wavetal
                 cp plugin/builds/linux_vst/build/Vitality+Minus.vst3/Contents/x86_64-linux/Vitality+Minus.so $out/vst3/wavetal.so
                 '';
               });
 

in
{
  imports =
    [ # Include the results of the hardware scan.
      ./hardware-configuration.nix
    ];
  home-manager = {
    useGlobalPkgs = true;
  };
  boot.kernelModules = [ "snd-seq" "snd-rawmidi" ];
  # Use the systemd-boot EFI boot loader.
  boot.loader.systemd-boot.enable = true;
  boot.loader.efi.canTouchEfiVariables = true;
  # Add ZFS support.
  boot.supportedFilesystems = ["zfs"]; 
  boot.zfs.requestEncryptionCredentials = true; 
  boot.initrd.postDeviceCommands = lib.mkAfter ''
    zfs rollback -r rpool/local/root@blank
  '';
  networking.hostId = "c9636f0f";
  networking.hostName = "loki"; # Define your hostname.
  #networking.wireless.enable = true;  # Enables wireless support via wpa_supplicant.
  networking.networkmanager.enable = true;
  # Configure network proxy if necessary
  # networking.proxy.default = "http://user:password@proxy:port/";
  # networking.proxy.noProxy = "127.0.0.1,localhost,internal.domain";
  security.sudo.extraConfig = ''
    moritz  ALL=(ALL) NOPASSWD: ${pkgs.systemd}/bin/systemctl
    '';
  musnix = {
    enable = true;
  }; 
 # Select internationalisation properties.
  programs.fuse.userAllowOther = true; 
  i18n = {
     consoleFont = "Lat2-Terminus16";
     consoleKeyMap = "us";
     defaultLocale = "en_US.UTF-8";
   };

  # Set your time zone.
   time.timeZone = "America/Chicago";

  # List packages installed in system profile. To search, run:
  # $ nix search wget
   environment.systemPackages = with pkgs; [
    csound  alsaLib vim git firefox ardour wavetal qjackctl pavucontrol jack_capture
   ];
  # Some programs need SUID wrappers, can be configured further or are
  # started in user sessions.
  # programs.mtr.enable = true;
  # programs.gnupg.agent = { enable = true; enableSSHSupport = true; };

  # List services that you want to enable:
  # Enable the OpenSSH daemon.
  # services.openssh.enable = true;
  services.tlp.enable = true;
  
  # ZFS services
  services.zfs.autoSnapshot.enable = true;
  services.zfs.autoScrub.enable = true;

  # Open ports in the firewall.
  # networking.firewall.allowedTCPPorts = [ ... ];
  # networking.firewall.allowedUDPPorts = [ ... ];
  # Or disable the firewall altogether.
  # networking.firewall.enable = false;

  # Enable CUPS to print documents.
  # services.printing.enable = true;

  nixpkgs.config = {
    allowBroken = true;
    allowUnfree = true; 
  };
  
  # Enable sound.
   hardware.enableAllFirmware = true;
  # Enable the X11 windowing system.
   services.xserver.enable = true;
   services.xserver.layout = "us";
   services.xserver.xkbOptions = "eurosign:e";

  # Enable touchpad support.
   services.xserver.libinput.enable = true;

  # Enable the KDE Desktop Environment.
   services.xserver.displayManager.sddm.enable = true;
   services.xserver.desktopManager.plasma5.enable = true;
   nix = {
    package = pkgs.nixFlakes;
    extraOptions = ''
      experimental-features = nix-command flakes
      substituters = https://cache.nixos.org https://static-haskell-nix.cachix.org https://hydra.iohk.io 
      trusted-public-keys = cache.nixos.org-1:6NCHdD59X431o0gWypbMrAURkbJ16ZPMQFGspcDShjY= static-haskell-nix.cachix.org-1:Q17HawmAwaM1/BfIxaEDKAxwTOyRVhPG5Ji9K3+FvUU= hydra.iohk.io:f/Ea+s+dFdN+3Y/G+FDgSq+a5NEWhJGzdjvKNGv0/EQ=
    '';
   };

services.jack = {
    jackd = {
      enable = true;
      # To obtain a valid device argument run `aplay -l`
      #     - `hw` prefix should be always there
      #     - `1` is a card number
      #     - `0` is a device number
      # Example (card 1, device 0)
      # card 1: USB [Scarlett 2i2 USB], device 0: USB Audio [USB Audio]
      #   Subdevices: 0/1
      #   Subdevice #0: subdevice #0
      extraOptions = [ "-dalsa" "--device" "hw:1,0" ];
      package = pkgs.jack2Full;
    };
  };

  hardware.pulseaudio = {
    enable = true;
    package = pkgs.pulseaudioFull;
    extraModules = [ pkgs.pulseaudio-modules-bt ];
    support32Bit = true;
  };



  # Must disable pulseaudio to allow for the pipewire pulseaudio emulation.

  # Define a user account. Don't forget to set a password with ‘passwd’.
   users.users.orlyn = {
     isNormalUser = true;
     extraGroups = [ "wheel" "networkmanager" "audio" "video" "disk" "jackaudio" "realtime" ]; # Enable ‘sudo’ for the user.
     initialPassword = "loyde1029";
     shell = pkgs.ion;
   };


  # This value determines the NixOS release with which your system is to be
  # compatible, in order to avoid breaking some software such as database
  # servers. You should change this only after NixOS release notes say you
  # should.
}


