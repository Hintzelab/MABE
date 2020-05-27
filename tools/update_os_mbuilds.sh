## this gets the latest of each binary
for filename in lin_build osx_build win_build.exe; do
  curl --output $filename -L "https://github.com/JorySchossau/mabe-build-tool/releases/latest/download/${filename}";
done
