{
  stdenv,
  fetchurl,
}:
stdenv.mkDerivation {
  name = "combo-lib";
  srcs = [
    (fetchurl {
      url = "https://hjemmesider.diku.dk/~pisinger/combo.c";
      sha256 = "sha256-3NkAzOURdXsAY3rygmwdoju6MGce7KibqESejA0SM3g=";
    })
    (fetchurl {
      url = "https://hjemmesider.diku.dk/~pisinger/combo.h";
      sha256 = "sha256-ci3jIbmkeeSHxDITaiPQpKud1D7lEhw3uDWaELmPS+4=";
    })
  ];

  unpackPhase = ''
    for f in $srcs; do
      cp $f $(stripHash $f)
    done
  '';

  buildPhase = ''
    gcc -O3 -c combo.c -o combo.o
    ar rcs libcombo.a combo.o
  '';

  installPhase = ''
    mkdir -p $out/include $out/lib
    cp combo.h $out/include/
    cp libcombo.a $out/lib/
  '';
}
