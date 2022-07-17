{ pkgs, lib, ... }:

{
  programs.home-manager.enable = true;
  home.packages = [
  pkgs.neovim
  pkgs.firefox
  pkgs.alacritty
  pkgs.gcc
  pkgs.tmux
  ];
  programs.neovim ={
     viAlias = true;
     vimAlias = true;
  };
  programs.zsh = {
   enable = true;
   shellAliases = {
    g = "git";
   };
  };
}

