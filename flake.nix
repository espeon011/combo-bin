{
  description = "C project using Pisinger's combo algorithm";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = {
    self,
    nixpkgs,
    flake-utils,
  }:
    flake-utils.lib.eachDefaultSystem (
      system: let
        # pkgs = nixpkgs.legacyPackages.${system};
        pkgs = import nixpkgs {inherit system;};

        # combo knapsack solver
        combo = pkgs.callPackage ./combo.nix {};

        drv = pkgs.stdenv.mkDerivation {
          pname = "combo";
          version = "0.1.0";
          src = ./.;
          buildInputs = [combo];
          buildPhase = ''
            gcc -O3 main.c -o $pname -lcombo
          '';
          installPhase = ''
            mkdir -p $out/bin
            cp $pname $out/bin/
          '';
        };
      in {
        packages.default = drv;
        apps.default = flake-utils.lib.mkApp {
          drv = drv;
          name = drv.pname;
        };
        devShells.default = pkgs.mkShell {
          inputsFrom = [drv];
          packages = [pkgs.clang-tools pkgs.gdb pkgs.valgrind];
        };
      }
    );
}
