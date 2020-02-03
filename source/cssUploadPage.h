const char cssUploadPage[] PROGMEM = R"=====(
<html>
   <head>
      <title>espArtNetNode CSS Upload</title>
   </head>
   <body>
      Select and upload your CSS file.  This will overwrite any previous uploads but you can restore the default below.<br /><br />
      <form method='POST' action='/style_upload' enctype='multipart/form-data'><input type='file' name='css'><input type='submit' value='Upload New CSS'></form>
      <br /><a href='/style_delete'>Restore default CSS</a>
   </body>
</html>
)=====";
