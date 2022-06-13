{
inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-22.05";
    musnix.url = "github:musnix/musnix";
    home-manager.url = "github:nix-community/home-manager/master";
    impermanence.url = "github:nix-community/impermanence/master";
    deploy-rs.url = "github:serokell/deploy-rs";
    flake-utils.url = "github:numtide/flake-utils";
  };
  outputs = { self, nixpkgs,flake-utils, musnix, home-manager, impermanence, deploy-rs }:
 {
    # replace 'joes-desktop' with your hostname here.
    nixosConfigurations.loki = nixpkgs.lib.nixosSystem {
      system = "x86_64-linux";
      modules = [
  home-manager.nixosModules.home-manager
{
    home-manager.useGlobalPkgs = true;
    home-manager.useUserPackages = true;
    home-manager.users.orlyn = import ./users/orlyn/home.nix; 
}
impermanence.nixosModules.impermanence{
   environment.persistence."/persist" = {
    directories = [
      "/etc/nixos"
      "/etc/NetworkManager/system-connections"
      "/music"
      "/home/orlyn/code"
    ];
    files = [
     "/etc/machine-id"
     "/home/orlyn/.background-image"
     "/.background-image"
     ];
};
}
   musnix.nixosModules.musnix
  
  ./machines/loki/configuration.nix ];
    };
       deploy.nodes.loki.hostname = "localhost";
       deploy.nodes.loki.profiles.system = {
        user = "root";
        path = deploy-rs.lib.x86_64-linux.activate.nixos self.nixosConfigurations.loki;
    };
     checks = builtins.mapAttrs (system: deployLib: deployLib.deployChecks self.deploy) deploy-rs.lib;
  };
}

