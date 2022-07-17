{
  inputs.nixpkgs.url = github:NixOS/nixpkgs/nixos-22.05;
  inputs.home-manager.url = github:nix-community/home-manager/release-22.05;
  inputs.neovim-flake.url = github:LBjerke/neovim-flake; 
  outputs = { self, nixpkgs,home-manager, neovim-flake,...}@attrs: {
    homeManagerConfigurations = 

    let
       overlays = [(_: _: {neovim = neovim-flake.defaultPackage.x86_64-linux;})];
     in
     {
      "loki" = home-manager.lib.homeManagerConfiguration {
       system = "x86_64-linux";
       configuration = {
       imports = [./nix/home/loki.nix];
       nixpkgs = {
       inherit overlays;
       config.allowUnfree = true;
       };
       };
    #    extraModules = [ ./nix/home/loki.nix];
       homeDirectory = "/home/loki";
       username = "loki";
      extraSpecialArgs = {inherit nixpkgs;};
    };
    };
    nixosConfigurations.nixos = nixpkgs.lib.nixosSystem {
      system = "x86_64-linux";
      specialArgs = attrs;
      modules = [ ./configuration.nix ];
    };
  };
}

