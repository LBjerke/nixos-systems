module "deploy_nixos" {
  source               = "github.com/tweag/terraform-nixos//deploy_nixos?ref=646cacb12439ca477c05315a7bfd49e9832bc4e3"
  nixos_config = "./flake.nix"
  flake = true
  target_user          = "orlyn"
  target_host          = "localhost"
  ssh_private_key_file = pathexpand("/home/orlyn/.ssh/id_rsa.pub")
}
