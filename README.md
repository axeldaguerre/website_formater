# website_formater WIP
 HTML parser which is simple, by consequence "fast": 220 files of 3500 bytes / sec.
 User will have simple error quite accurate on the type of error and the byte offset from the file (no line and column because I do think it's pointless, prone to error and add complexity for no value -> just give a link with byte offset and use OS API that uses only that).
 
# Goals
1 - Make this tool easy to integrate to code editor like vscode, emacs or other.
2 - Make it very fast, don't want to wait 5 sec for an error to come up
3 - Make it cross platform, at least Windows + Linux

# End Goal 

The end goal of this tool is to integrate it to a broader tool sets that will permit to annotate *rapidly* pdf or the like and extract them in a simple file format, create simple text file that will be translated to html (md like).
The end tool will have to be fast, cross platform and avoid bugs or at least without good informations about them. *It must be able to run on a capable potatoe (idest ereader).
 
This repository is *highly* inspired by https://github.com/EpicGames/raddebugger I found really well organized, cross platform and easy to understand.