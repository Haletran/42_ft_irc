# 42_FT_IRC

=> Project made in collaboration with [@gnoumalin](https://github.com/gnoumalin)

This project is about creating your own IRC server.

## BOT animation

Script to generate frames for animation :)
> Note :
> set scrollback lines to `30`
> change font to `JetBrains Mono` for better rendering of black color

```bash
# Inspired by this tutorial : https://gist.github.com/aishenreemo/1902ce9d938202fe03ae9377ac69a7e9
#ascii image converter link : https://github.com/TheZoraiz/ascii-image-converter/releases/download/v1.13.1/ascii-image-converter_Linux_amd64_64bit.tar.gz
mkdir animation
cd animation

yt-dlp -f "bestvideo[ext=mp4]+bestaudio[ext=m4a]/best[ext=mp4]/best" https://www.youtube.com/watch\?v\=FtutLA63Cp8\&ab_channel\=kasidid2 -o bad_apple.mp4
ffmpeg -i bad_apple.mp4 bad_apple.mp3
mkdir -p frames
ffmpeg -i bad_apple.mp4 -vf fps=10 frames/out%05d.jpg

cd frames
mkdir -p ../frames_ascii

for file in *.jpg; do
    ascii-image-converter "$file" --braille --dimensions 90,30 > "../frames_ascii/${file%.jpg}.txt"
done
```
