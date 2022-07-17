# Edit this configuration file to define what should be installed on
# your system.  Help is available in the configuration.nix(5) man page
# and in the NixOS manual (accessible by running ‘nixos-help’).

{ config, pkgs, ... }:

{
  imports =
    [ # Include the results of the hardware scan.
      ./hardware-configuration.nix
    ];

  # Bootloader.
  boot.loader.systemd-boot.enable = true;
  boot.loader.efi.canTouchEfiVariables = true;
  boot.loader.efi.efiSysMountPoint = "/boot/efi";

  # Setup keyfile
  boot.initrd.secrets = {
    "/crypto_keyfile.bin" = null;
  };

  # Enable swap on luks
  boot.initrd.luks.devices."luks-3b6ffa7b-1f28-4aac-8d8f-cd9cfdd9622f".device = "/dev/disk/by-uuid/3b6ffa7b-1f28-4aac-8d8f-cd9cfdd9622f";
  boot.initrd.luks.devices."luks-3b6ffa7b-1f28-4aac-8d8f-cd9cfdd9622f".keyFile = "/crypto_keyfile.bin";

  networking.hostName = "loki"; # Define your hostname.
  # networking.wireless.enable = true;  # Enables wireless support via wpa_supplicant.

  # Configure network proxy if necessary
  # networking.proxy.default = "http://user:password@proxy:port/";
  # networking.proxy.noProxy = "127.0.0.1,localhost,internal.domain";

  # Enable networking
  networking.networkmanager.enable = true;

  # Set your time zone.
  time.timeZone = "America/Chicago";

  # Select internationalisation properties.
  i18n.defaultLocale = "en_US.utf8";

  # Enable the X11 windowing system.
  services.xserver.enable = true;

  # Enable the Pantheon Desktop Environment.
  services.xserver.displayManager.lightdm.enable = true;
  services.xserver.desktopManager.pantheon.enable = true;

  # Configure keymap in X11
  services.xserver = {
    layout = "us";
    xkbVariant = "";
  };

  # Enable CUPS to print documents.
  services.printing.enable = true;

  # Enable sound with pipewire.
  sound.enable = true;
  hardware.pulseaudio.enable = false;
  security.rtkit.enable = true;
  services.pipewire = {
    enable = true;
    alsa.enable = true;
    alsa.support32Bit = true;
    pulse.enable = true;
    # If you want to use JACK applications, uncomment this
    #jack.enable = true;

    # use the example session manager (no others are packaged yet so this is enabled by default,
    # no need to redefine it in your config for now)
    #media-session.enable = true;
  };

  # Enable touchpad support (enabled default in most desktopManager).
  # services.xserver.libinput.enable = true;

  # Define a user account. Don't forget to set a password with ‘passwd’.
  users.users.loki = {
    isNormalUser = true;
    shell = pkgs.zsh;
    description = "Loki";
    extraGroups = [ "networkmanager" "wheel" ];
    openssh.authorizedKeys.keys = [
    "ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABgQDPCwHtt+q7STpRwu8XwyyFnERQMujfoBZO/AHl2//gfu2v+FcvGBr15X2782aJrOwuZJmFs/auEk6KOHpCbyDJCNTgPD6tleTqTJNqvB7jTNAb//rMy6J6x08lvojj2dyJh0MIlsssywGGBAepHlqwoB1JvoSZm4lRTqVuby3KtHTnwuxbxKmjl0L6Msuf2C0E7zUyJOLYPmgPHTrSq6oqNmGCFHwV2szqATSvg+yTusVanspRCghXxYRhVgFolB83O9igvVGO64EzTXxO1AMl711OJsAuvgzbiTePH47v3pnEpgIRABNasIpJR7El+SKekUdoeVLRD/SNtRvUAkghryg1XxxqxqkLNZmca2xGUZazWOxITap/fNyZvcmjuRPGGNkrDoBPbT/6wQrPh5bGzePFZm13CMsfIaZKGijRJzwhjokLX4ER8gONYOxjxAFFSGroBCnFf8i68n5rpatyQzW080CNTkq50XgKxcu0dKtq48zvLuYuzkfU1mo4nW0= loki@loki"
    ];
  };

  # Allow unfree packages
  nixpkgs.config.allowUnfree = true;
    nix = {
    package = pkgs.nixFlakes; # or versioned attributes like nixVersions.nix_2_8
    extraOptions = ''
      experimental-features = nix-command flakes
    '';
   };
  # List packages installed in system profile. To search, run:
  # $ nix search wget
  environment.systemPackages = with pkgs; [
    neovim # Do not forget to add an editor to edit configuration.nix! The Nano editor is also installed by default.
    git
    tmux

wget
alacritty
    vlc
    firefox
  ];

  # Some programs need SUID wrappers, can be configured further or are
  # started in user sessions.
  # programs.mtr.enable = true;
  # programs.gnupg.agent = {
  #   enable = true;
  #   enableSSHSupport = true;
  # };

  # List services that you want to enable:

  # Enable the OpenSSH daemon.
   services.openssh.enable = true;

  # Open ports in the firewall.
  # networking.firewall.allowedTCPPorts = [ ... ];
  # networking.firewall.allowedUDPPorts = [ ... ];
  # Or disable the firewall altogether.
  # networking.firewall.enable = false;

  # This value determines the NixOS release from which the default
  # settings for stateful data, like file locations and database versions
  # on your system were taken. It‘s perfectly fine and recommended to leavecatenate(variables, "bootdev", bootdev)
  # this value at the release version of the first install of this system.
  # Before changing this value read the documentation for this option
  # (e.g. man configuration.nix or on https://nixos.org/nixos/options.html).
  system.stateVersion = "22.05"; # Did you read the comment?

}
