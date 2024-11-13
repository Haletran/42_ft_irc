# 42_FT_IRC

[Schema for the project](https://excalidraw.com/#json=txZRLVuHd0XKJGnX15tuu,bFvyTwCpZ81q7YzrV7C1fQ)


Script to generate frames for animation :)


```bash
# Inspired by this tutorial : https://gist.github.com/aishenreemo/1902ce9d938202fe03ae9377ac69a7e9

mkdir animation
cd animation

yt-dlp -f "bestvideo[ext=mp4]+bestaudio[ext=m4a]/best[ext=mp4]/best" https://www.youtube.com/watch\?v\=FtutLA63Cp8\&ab_channel\=kasidid2 -o bad_apple.mp4
ffmpeg -i bad_apple.mp4 bad_apple.mp3
mkdir -p frames
ffmpeg -i bad_apple.mp4 -vf fps=10 frames/out%05d.jpg

cd frames
mkdir -p ../frames_ascii

for file in *.jpg; do
    ascii-image-converter "$file" --braille > "../frames_ascii/${file%.jpg}.txt"
done
```
