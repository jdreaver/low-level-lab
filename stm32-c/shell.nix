{ system ? builtins.currentSystem }:

(builtins.getFlake (toString ../flake)).devShells.${system}.stm32-c
