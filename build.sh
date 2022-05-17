#! /usr/bin/env bash 
  
 # Shows the terminal

build_ci_system() { 
   nix build .#nixosConfigurations.loki.config.system.build.toplevel 
 } 
 

case $1 in 
   "ci-home") 
     build_ci_home;; 
   "ci-system") 
     build_ci_system;; 
   "fresh-install") 
     fresh_install;; 
   "pre-home") 
     pre_home;; 
   "post-home") 
     post_home;; 
   *) 
     echo "Invalid option. Expected 'ci-home', 'ci-system', 'pre_home', 'post_home' or 'fresh-install'";; 
 esac
