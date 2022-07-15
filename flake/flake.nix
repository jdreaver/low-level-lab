{
  inputs = {
    nixpkgs-unstable.url = "github:nixos/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs-unstable }:
    let
      pkgs = import nixpkgs-unstable { system = "x86_64-linux"; config = { allowUnfree = true; }; };
    in {
      devShells.x86_64-linux.default = pkgs.mkShell {
        buildInputs = with pkgs; [
          # Java, to run nand2tetris tools
          jre

          # Rust
          cargo
          rustc
          clippy
          rustfmt
          rust-analyzer

          # C
          gcc
          gcc.man
          gdb
          ddd # Nice GUI debugger
          valgrind
          bear # Generates compile_commands.json

          # ASM
          nasm
          # N.B. We use yasm instead of nasm because nasm 2.15.05 (Aug 28 2020)
          # and gdb 12.1 (May 2022) don't play nice together. There doesn't seem
          # to be any source location information included in the resulting
          # binary. Specifically, we have to use "ni" instead of "n" for
          # stepping, and any command that tries to inspect the source (like
          # layout src, or TUI mode), says "[No Source Available]".
          yasm

          # Python
          python3

          # Misc
          unzip

          # Kernel tools
          coccinelle
          sparse
        ];
      };
    };
}
