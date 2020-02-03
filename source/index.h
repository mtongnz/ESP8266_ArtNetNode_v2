const char mainPage[] PROGMEM = R"=====(
<!DOCTYPE html>
<meta content='text/html; charset=utf-8' http-equiv=Content-Type />
<title>ESP8266 ArtNetNode Config</title>
<meta content='Matthew Tong - http://github.com/mtongnz/' name=DC.creator />
<meta content=en name=DC.language />
<meta content='width=device-width,initial-scale=1' name=viewport />
<link href=style.css rel=stylesheet />
<div id=page>
   <div class=inner>
      <div class=mast>
         <div class=title>
            esp8266
            <h1>ArtNet & sACN</h1>
            to
            <h1>DMX & LED Pixels</h1>
         </div>
         <ul class=nav>
            <li class=first><a href='javascript: menuClick(1)'>Device Status</a>
            <li><a href='javascript: menuClick(2)'>WiFi</a>
            <li><a href='javascript: menuClick(3)'>IP & Name</a>
            <li><a href='javascript: menuClick(4)'>Port A</a>
            <li><a href='javascript: menuClick(5)'>Port B</a>
            <li><a href='javascript: menuClick(6)'>Scenes</a>
            <li><a href='javascript: menuClick(7)'>Firmware</a>
            <li class=last><a href='javascript: reboot()'>Reboot</a>
         </ul>
         <div class=author><i>Design by</i> Matthew Tong</div>
      </div>
      <div class='main section'>
         <div class=hide name=error>
            <h2>Error</h2>
            <p class=center>There was an error communicating with the device. Refresh the page and try again.
         </div>
         <div class=show name=sections>
            <h2>Fetching Data</h2>
            <p class=center>Fetching data from device. If this message is still here in 15 seconds, try refreshing the page or clicking the menu option again.
         </div>
         <div class=hide name=sections>
            <h2>Device Status</h2>
            <p class=left>Device Name:
            <p class=right name=nodeName>
            <p class=left>MAC Address:
            <p class=right name=macAddress>
            <p class=left>Wifi Status:
            <p class=right name=wifiStatus>
            <p class=left>IP Address:
            <p class=right name=ipAddressT>
            <p class=left>Subnet Address:
            <p class=right name=subAddressT>
            <p class=left>Port A:
            <p class=right name=portAStatus>
            <p class=left>Port B:
            <p class=right name=portBStatus>
            <p class=left>Scene Storage:
            <p class=right name=sceneStatus>
            <p class=left>Firmware:
            <p class=right name=firmwareStatus>
         </div>
         <div class=hide name=sections>
            <input name=save type=button value='Save Changes'/>
            <h2>WiFi Settings</h2>
            <p class=left>MAC Address:
            <p class=right name=macAddress>
            <p class=spacer>
            <p class=left>Wifi SSID:
            <p class=right><input type=text name=wifiSSID />
            <p class=left>Password:
            <p class=right><input type=text name=wifiPass />
            <p class=spacer>
            <p class=left>Hotspot SSID:
            <p class=right><input type='text' name='hotspotSSID' />
            <p class=left>Password:
            <p class=right><input type=text name=hotspotPass />
            <p class=left>Start Delay:
            <p class=right><input name=hotspotDelay type=number min=0 max=180 class=number /> (seconds)
            <p class=spacer>
            <p class=left>Stand Alone:
            <p class=right><input name=standAloneEnable type=checkbox value=true />
            <p class=right>In normal mode, the hotspot will start after <i>delay</i> seconds if the main WiFi won't connect. If no users connect, the device will reset and attempt the main WiFi again. This feature is purely for changing settings and ArtNet data is ignored.
            <p class=right>Stand alone mode disables the primary WiFi connection and allows ArtNet data to be received via the hotspot connection.
         </div>
         <div class=hide name=sections>
            <input name=save type=button value='Save Changes'/>
            <h2>IP & Node Name</h2>
            <p class='left'>Short Name:</p>
            <p class=right><input type=text name=nodeName />
            <p class=left>Long Name:
            <p class=right><input type=text name=longName />
            <p class=spacer>
            <p class=left>Enable DHCP:
            <p class=right><input name=dhcpEnable type=checkbox value=true />
            <p class=left>IP Address:
            <p class=right><input name=ipAddress type=number min=0 max=255 class=number /> <input name=ipAddress type=number min=0 max=255 class=number /> <input name=ipAddress type=number min=0 max=255 class=number /> <input name=ipAddress type=number min=0 max=255 class=number />
            <p class=left>Subnet Address:
            <p class=right><input name=subAddress type=number min=0 max=255 class=number /> <input name=subAddress type=number min=0 max=255 class=number /> <input name=subAddress type=number min=0 max=255 class=number /> <input name=subAddress type=number min=0 max=255 class=number />
            <p class=left>Gateway Address:
            <p class=right><input name=gwAddress type=number min=0 max=255 class=number /> <input name=gwAddress type=number min=0 max=255 class=number /> <input name=gwAddress type=number min=0 max=255 class=number /> <input name=gwAddress type=number min=0 max=255 class=number />
            <p class=left>Broadcast Address:
            <p class=right name=bcAddress>
            <p class=center>These settings only affect the main WiFi connection. The hotspot will always have DHCP enabled and an IP of <b>2.0.0.1</b>
         </div>
         <div class=hide name=sections>
            <input name=save type=button value='Save Changes'/>
            <h2>Port A Settings</h2>
            <p class=left>Port Type:
            <p class=right>
               <select class=select name=portAmode>
                  <option value=0>DMX Output
                  <option value=1>DMX Output with RDM
                  <option value=2>DMX Input
                  <option value=3>LED Pixels - WS2812
               </select>
            <p class=left>Protocol:
            <p class=right>
               <select class=select name=portAprot>
                  <option value=0>Artnet v4
                  <option value=1>sACN Unicast
                  <option value=2>sACN Multicast
               </select>
            <p class=left>Merge Mode:
            <p class=right>
               <select class=select name=portAmerge>
                  <option value=0>Merge LTP
                  <option value=1>Merge HTP
               </select>
            <p class=left>Net:
            <p class=right><input name=portAnet type=number min=0 max=127 class=number />
            <p class=left>Subnet:
            <p class=right><input name=portAsub type=number min=0 max=15 class=number />
            <p class=left>Universe:
            <p class=right><input type=number min=0 max=15 name=portAuni class=number /><span name=portApix> <input type=number min=0 max=15 name=portAuni class=number /> <input type=number min=0 max=15 name=portAuni class=number /> <input type=number min=0 max=15 name=portAuni class=number /></span></p>
            <p class=left>sACN Universe:
            <p class=right><input type=number min=0 max=63999 name=portAsACNuni class=number /> <input type=number min=1 max=63999 name=portAsACNuni class=number /> <input type=number min=1 max=63999 name=portAsACNuni class=number /> <input type=number min=1 max=63999 name=portAsACNuni class=number /></p>
            <span name=DmxInBcAddrA>
               <p class=left>Broadcast Address:
               <p class=right><input type=number min=0 max=255 name=dmxInBroadcast class=number /> <input type=number min=0 max=255 name=dmxInBroadcast class=number /> <input type=number min=0 max=255 name=dmxInBroadcast class=number /> <input type=number min=0 max=255 name=dmxInBroadcast class=number /></p>
            </span>
            <span name=portApix>
               <p class=left>Number of Pixels:
               <p class=right><input type=number min=0 max=680 name=portAnumPix class=number /> 680 max - 170 per universe</p>
               <p class=left>Mode:</p>
               <p class=right>
                  <select class=select name=portApixMode>
                     <option value=0>Pixel Mapping</option>
                     <option value=1>12 Channel FX</option>
                  </select>
               <p class=left>Start Channel:</p>
               <p class=right><input type=number name=portApixFXstart class=number min=1 max=501 /> FX modes only</p>
            </span>
         </div>
         <div class=hide name=sections>
            <input name=save type=button value='Save Changes'/>
            <h2>Port B Settings</h2>
            <p class=left>Port Type:
            <p class=right>
               <select class=select name=portBmode>
                  <option value=0>DMX Output
                  <option value=1>DMX Output with RDM
                  <option value=3>LED Pixels - WS2812
               </select>
            <p class=left>Protocol:
            <p class=right>
               <select class=select name=portBprot>
                  <option value=0>Artnet v4
                  <option value=1>sACN Unicast
                  <option value=2>sACN Multicast
               </select>
            <p class=left>Merge Mode:
            <p class=right>
               <select class=select name=portBmerge>
                  <option value=0>Merge LTP
                  <option value=1>Merge HTP
               </select>
            <p class=left>Net:
            <p class=right><input name=portBnet type=number min=0 max=127 class=number />
            <p class=left>Subnet:
            <p class=right><input name=portBsub type=number min=0 max=15 class=number />
            <p class=left>Universe:
            <p class=right><input type=number min=0 max=15 name=portBuni class=number /><span name=portBpix> <input type=number min=0 max=15 name=portBuni class=number /> <input type=number min=0 max=15 name=portBuni class=number /> <input type=number min=0 max=15 name=portBuni class=number /></span></p>
            <p class=left>sACN Universe:
            <p class=right><input type=number min=1 max=63999 name=portBsACNuni class=number /> <input type=number min=1 max=63999 name=portBsACNuni class=number /> <input type=number min=1 max=63999 name=portBsACNuni class=number /> <input type=number min=1 max=63999 name=portBsACNuni class=number /></p>
            <span name=portBpix>
               <p class=left>Number of Pixels:
               <p class=right><input type=number min=0 max=680 name=portBnumPix class=number /> 680 max - 170 per universe</p>
               <p class=left>Mode:</p>
               <p class=right>
                  <select class=select name=portBpixMode>
                     <option value=0>Pixel Mapping</option>
                     <option value=1>12 Channel FX</option>
                  </select>
               <p class=left>Start Channel:</p>
               <p class=right><input type=number name=portBpixFXstart class=number min=1 max=501 /> FX modes only</p>
            </span>
         </div>
         <div class=hide name=sections>
            <h2>Stored Scenes</h2>
            <p class=center>Not yet implemented
         </div>
         <div class=hide name=sections>
            <form action=/update enctype=multipart/form-data method=POST id=firmForm>
            <h2>Update Firmware</h2>
            <p class=left>Firmware:
            <p class=right name=firmwareStatus>
            <p class=right>
               <input name=update type=file id=update>
               <label for=update>
                  <svg height=17 viewBox='0 0 20 17' width=20 xmlns=http://www.w3.org/2000/svg>
                     <path d='M10 0l-5.2 4.9h3.3v5.1h3.8v-5.1h3.3l-5.2-4.9zm9.3 11.5l-3.2-2.1h-2l3.4 2.6h-3.5c-.1 0-.2.1-.2.1l-.8 2.3h-6l-.8-2.2c-.1-.1-.1-.2-.2-.2h-3.6l3.4-2.6h-2l-3.2 2.1c-.4.3-.7 1-.6 1.5l.6 3.1c.1.5.7.9 1.2.9h16.3c.6 0 1.1-.4 1.3-.9l.6-3.1c.1-.5-.2-1.2-.7-1.5z'/>
                  </svg>
                  <span>Choose Firmware</span>
               </label>
            <p class=right id=uploadMsg></p>
            <p class=right><input type=button class=submit value='Upload Now' id=fUp>
         </div>
      </div>
      <div class=footer>
         <p>Coding and hardware © 2016-2017 <a href=http://github.com/mtongnz/ >Matthew Tong</a>.
         <p>Released under <a href=http://www.gnu.org/licenses/ >GNU General Public License V3</a>.
      </div>
   </div>
</div>
<script>
var cl = 0;
var num = 0;
var err = 0;
var o = document.getElementsByName('sections');
var s = document.getElementsByName('save');
for (var i = 0, e; e = s[i++];) e.addEventListener('click', function() {
    sendData();
});
var u = document.getElementById('fUp');
var um = document.getElementById('uploadMsg');
var fileSelect = document.getElementById('update');
u.addEventListener('click', function() {
    uploadPrep()
});

function uploadPrep() {
    if (fileSelect.files.length === 0) return;
    u.disabled = !0;
    u.value = 'Preparing Device…';
    var x = new XMLHttpRequest();
    x.onreadystatechange = function() {
        if (x.readyState == XMLHttpRequest.DONE) {
            try {
                var r = JSON.parse(x.response)
            } catch (e) {
                var r = {
                    success: 0,
                    doUpdate: 1
                }
            }
            if (r.success == 1 && r.doUpdate == 1) {
                uploadWait()
            } else {
                um.value = '<b>Update failed!</b>';
                u.value = 'Upload Now';
                u.disabled = !1
            }
        }
    };
    x.open('POST', '/ajax', !0);
    x.setRequestHeader('Content-Type', 'application/json');
    x.send('{\"doUpdate\":1,\"success\":1}')
}

function uploadWait() {
    setTimeout(function() {
        var z = new XMLHttpRequest();
        z.onreadystatechange = function() {
            if (z.readyState == XMLHttpRequest.DONE) {
                try {
                    var r = JSON.parse(z.response)
                } catch (e) {
                    var r = {
                        success: 0
                    }
                }
                console.log('r=' + r.success);
                if (r.success == 1) {
                    upload()
                } else {
                    uploadWait()
                }
            }
        };
        z.open('POST', '/ajax', !0);
        z.setRequestHeader('Content-Type', 'application/json');
        z.send('{\"doUpdate\":2,\"success\":1}')
    }, 1000)
}
var upload = function() {
    u.value = 'Uploading… 0%';
    var data = new FormData();
    data.append('update', fileSelect.files[0]);
    var x = new XMLHttpRequest();
    x.onreadystatechange = function() {
        if (x.readyState == 4) {
            try {
                var r = JSON.parse(x.response)
            } catch (e) {
                var r = {
                    success: 0,
                    message: 'No response from device.'
                }
            }
            console.log(r.success + ': ' + r.message);
            if (r.success == 1) {
                u.value = r.message;
                setTimeout(function() {
                    location.reload()
                }, 15000)
            } else {
                um.value = '<b>Update failed!</b> ' + r.message;
                u.value = 'Upload Now';
                u.disabled = !1
            }
        }
    };
    x.upload.addEventListener('progress', function(e) {
        var p = Math.ceil((e.loaded / e.total) * 100);
        console.log('Progress: ' + p + '%');
        if (p < 100) u.value = 'Uploading... ' + p + '%';
        else u.value = 'Upload complete. Processing…'
    }, !1);
    x.open('POST', '/upload', !0);
    x.send(data)
};

function reboot() {
    if (err == 1) return;
    var r = confirm('Are you sure you want to reboot?');
    if (r != true) return;
    o[cl].className = 'hide';
    o[0].childNodes[0].innerHTML = 'Rebooting';
    o[0].childNodes[1].innerHTML = 'Please wait while the device reboots. This page will refresh shortly unless you changed the IP or Wifi.';
    o[0].className = 'show';
    err = 0;
    var x = new XMLHttpRequest();
    x.onreadystatechange = function() {
        if (x.readyState == 4) {
            try {
                var r = JSON.parse(x.response);
            } catch (e) {
                var r = {
                    success: 0,
                    message: 'Unknown error: [' + x.responseText + ']'
                };
            }
            if (r.success != 1) {
                o[0].childNodes[0].innerHTML = 'Reboot Failed';
                o[0].childNodes[1].innerHTML = 'Something went wrong and the device didnt respond correctly.Pleasetry again.';
            }
            setTimeout(function() {
                location.reload();
            }, 5000);
        }
    };
    x.open('POST', '/ajax', true);
    x.setRequestHeader('Content-Type', 'application/json');
    x.send('{\"reboot\":1,\"success\":1}');
}

function sendData() {
    var d = {
        'page': num
    };
    for (var i = 0, e; e = o[cl].getElementsByTagName('INPUT')[i++];) {
        var k = e.getAttribute('name');
        var v = e.value;
        if (k in d) continue;
        if (k == 'ipAddress' || k == 'subAddress' || k == 'gwAddress' || k == 'portAuni' || k == 'portBuni' || k == 'portAsACNuni' || k == 'portBsACNuni' || k == 'dmxInBroadcast') {
            var c = [v];
            for (var z = 1; z < 4; z++) {
                c.push(o[cl].getElementsByTagName('INPUT')[i++].value);
            }
            d[k] = c;
            continue;
        }
        if (e.type === 'text') d[k] = v;
        if (e.type === 'number') {
            if (v == '') v = 0;
            d[k] = v;
        }
        if (e.type === 'checkbox') {
            if (e.checked) d[k] = 1;
            else d[k] = 0;
        }
    }
    for (var i = 0, e; e = o[cl].getElementsByTagName('SELECT')[i++];) {
        d[e.getAttribute('name')] = e.options[e.selectedIndex].value;
    }
    d['success'] = 1;
    var x = new XMLHttpRequest();
    x.onreadystatechange = function() {
        handleAJAX(x);
    };
    x.open('POST', '/ajax');
    x.setRequestHeader('Content-Type', 'application/json');
    x.send(JSON.stringify(d));
    console.log(d);
}

function menuClick(n) {
    if (err == 1) return;
    num = n;
    setTimeout(function() {
        if (cl == num || err == 1) return;
        o[cl].className = 'hide';
        o[0].className = 'show';
        cl = 0;
    }, 100);
    var x = new XMLHttpRequest();
    x.onreadystatechange = function() {
        handleAJAX(x);
    };
    x.open('POST', '/ajax');
    x.setRequestHeader('Content-Type', 'application/json');
    x.send(JSON.stringify({page:num, success:1}));
}

function handleAJAX(x) {
    if (x.readyState == XMLHttpRequest.DONE) {
        if (x.status == 200) {
            var response = JSON.parse(x.responseText);
            console.log(response);
            if (!response.hasOwnProperty('success')) {
                err = 1;
                o[cl].className = 'hide';
                document.getElementsByName('error')[0].className = 'show';
                return;
            }
            if (response['success'] != 1) {
                err = 1;
                o[cl].className = 'hide';
                document.getElementsByName('error')[0].getElementsByTagName('P')[0].innerHTML = response['message'];
                document.getElementsByName('error')[0].className = 'show';
                return;
            }
            if (response.hasOwnProperty('message')) {
                for (var i = 0, e; e = s[i++];) {
                    e.value = response['message'];
                    e.className = 'showMessage'
                }
                setTimeout(function() {
                    for (var i = 0, e; e = s[i++];) {
                        e.value = 'Save Changes';
                        e.className = ''
                    }
                }, 5000);
            }
            o[cl].className = 'hide';
            o[num].className = 'show';
            cl = num;
            for (var key in response) {
                if (response.hasOwnProperty(key)) {
                    var a = document.getElementsByName(key);
                    if (key == 'ipAddress' || key == 'subAddress') {
                        var b = document.getElementsByName(key + 'T');
                        for (var z = 0; z < 4; z++) {
                            a[z].value = response[key][z];
                            if (z == 0) b[0].innerHTML = '';
                            else b[0].innerHTML = b[0].innerHTML + ' . ';
                            b[0].innerHTML = b[0].innerHTML + response[key][z];
                        }
                        continue;
                    } else if (key == 'bcAddress') {
                        for (var z = 0; z < 4; z++) {
                            if (z == 0) a[0].innerHTML = '';
                            else a[0].innerHTML = a[0].innerHTML + ' . ';
                            a[0].innerHTML = a[0].innerHTML + response[key][z];
                        }
                        continue;
                    } else if (key == 'gwAddress' || key == 'dmxInBroadcast' || key == 'portAuni' || key == 'portBuni' || key == 'portAsACNuni' || key == 'portBsACNuni') {
                        for (var z = 0; z < 4; z++) {
                            a[z].value = response[key][z];
                        }
                        continue
                    }
                    if (key == 'portAmode') {
                        var b = document.getElementsByName('portApix');
                        var c = document.getElementsByName('DmxInBcAddrA');
                        if (response[key] == 3) {
                            b[0].style.display = '';
                            b[1].style.display = '';
                        } else {
                            b[0].style.display = 'none';
                            b[1].style.display = 'none';
                        }
                        if (response[key] == 2) {
                            c[0].style.display = '';
                        } else {
                            c[0].style.display = 'none';
                        }
                    } else if (key == 'portBmode') {
                        var b = document.getElementsByName('portBpix');
                        if (response[key] == 3) {
                            b[0].style.display = '';
                            b[1].style.display = '';
                        } else {
                            b[0].style.display = 'none';
                            b[1].style.display = 'none';
                        }
                    }
                    for (var z = 0; z < a.length; z++) {
                        switch (a[z].nodeName) {
                            case 'P':
                            case 'DIV':
                                a[z].innerHTML = response[key];
                                break;
                            case 'INPUT':
                                if (a[z].type == 'checkbox') {
                                    if (response[key] == 1) a[z].checked = true;
                                    else a[z].checked = false;
                                } else a[z].value = response[key];
                                break;
                            case 'SELECT':
                                for (var y = 0; y < a[z].options.length; y++) {
                                    if (a[z].options[y].value == response[key]) {
                                        a[z].options.selectedIndex = y;
                                        break;
                                    }
                                }
                                break;
                        }
                    }
                }
            }
        } else {
            err = 1;
            o[cl].className = 'hide';
            document.getElementsByName('error')[0].className = 'show';
        }
    }
}
var update = document.getElementById('update');
var label = update.nextElementSibling;
var labelVal = label.innerHTML;
update.addEventListener('change', function(e) {
    var fileName = e.target.value.split('\\\\').pop();
    if (fileName) label.querySelector('span').innerHTML = fileName;
    else label.innerHTML = labelVal;
    update.blur();
});
document.onkeydown = function(e) {
    if (cl < 2 || cl > 6) return;
    var e = e || window.event;
    if (e.keyCode == 13) sendData();
};
menuClick(1);
</script>
</body>
</html>
)=====";
