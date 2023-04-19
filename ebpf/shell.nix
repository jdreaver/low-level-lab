{ system ? builtins.currentSystem }:

(builtins.getFlake (toString ../flake)).devShells.${system}.ebpf
