# 42_FT_IRC

[Schema for the project](https://excalidraw.com/#json=txZRLVuHd0XKJGnX15tuu,bFvyTwCpZ81q7YzrV7C1fQ)


## ERRORS

- [ ] : if user leaves the channel send a message to every user on the channel that he leaves ✅
- [ ] : if the user have the same username then it think that both user has leave the channel ✅ fixed (i think) 2 users cant have the same username now (i add 1...etc to the username)
- [ ] : if the user leaves channel he can't rejoin it cause it says he is alredy in channel (jai essaye de regler le probleme mais jai pas reussi, ca vient probablement du fait que de dans PART on doit le delete d'un vector qu'est pas le bon et dans alreadyinchannel check le mauvais vector jsp)
- [ ] : refactor free because such a mess
- [ ] : check if the BOT can make leaks
- [ ] : fix QUIT command
- [ ] : Parsing of channel name, username according to RFC ✅
- [ ] : CTRL+D not working correctly (a tester jai ajoute un truc mais pas sur qu ca fonctionne)

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
