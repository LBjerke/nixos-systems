{
inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    musnix.url = "github:musnix/musnix";
    home-manager.url = "github:nix-community/home-manager/master";
    impermanence.url = "github:nix-community/impermanence/master";
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
    home-manager.users.orlyn = {  
    programs.home-manager.enable = true;
    services.network-manager-applet.enable = true;
  };
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
     "/etc/nixos/configuration.nix"
     "/etc/nixos/hardware-configuration.nix"
     ];
};
}
   musnix.nixosModules.musnix
  
  ./configuration.nix ];
    };
  };
}

