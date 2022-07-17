{
inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    musnix.url = "github:musnix/musnix";
    home-manager.url = "github:nix-community/home-manager/master";
  };
  outputs = { self, nixpkgs, musnix, home-manager, impermanence }:
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
   musnix.nixosModules.musnix
  
  ./machines/loki/configuration.nix ];
    };
  };
}

