{pkgs ? import <nixpkgs> {}, ...}:
pkgs.mkShell {
  packages = with pkgs; [
    gcc-arm-embedded
    avrdude
    dfu-programmer
    dfu-util
    uv
    pkgsCross.avr.buildPackages.gcc
  ];
  shellHook = ''
    export PATH="$HOME/.local/bin:$PATH"
  '';
}
