{ config, pkgs,lib,vimUtils, ... }:
{
  # Let Home Manager install and manage itself.
  programs.home-manager.enable = true;

  # Home Manager needs a bit of information about you and the
  # paths it should manage.
  home.username = "orlyn";
  home.homeDirectory = "/home/orlyn";
  programs.ion = {
   enable = true;
  };
 programs.starship = {
   enable = true;
  enableIonIntegration = true;
  };
  services.network-manager-applet.enable = true;
  home.packages = [
  pkgs.tmux
  pkgs.ardour
  pkgs.vim
  pkgs.git
  pkgs.emacs
  pkgs.vlc
  pkgs.terraform
  ];

  programs.git = {
   enable = true;
   userName = "LBjerke";
   userEmail = "loyde.b.bjerke@gmail.com";
  };
  # This value determines the Home Manager release that your
  # configuration is compatible with. This helps avoid breakage
  # when a new Home Manager release introduces backwards
  # incompatible changes.
  #
  # You can update Home Manager without changing this value. See
  # the Home Manager release notes for a list of state version
  # changes in each release.
}

