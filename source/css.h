const char css[] PROGMEM = R"=====(
.author,.title,ul.nav a{
    text-align:center
}
.author i,.show,.title h1,ul.nav a{
    display:block
}
input,ul.nav a:hover{
    background-color:#DADADA
}
a,abbr,acronym,address,applet,b,big,blockquote,body,caption,center,cite,code,dd,del,dfn,div,dl,dt,em,fieldset,font,form,h1,h2,h3,h4,h5,h6,html,i,iframe,img,ins,kbd,label,legend,li,object,ol,p,pre,q,s,samp,small,span,strike,strong,sub,sup,table,tbody,td,tfoot,th,thead,tr,tt,u,ul,var{
    margin:0;
    padding:0;
    border:0;
    outline:0;
    font-size:100%;
    vertical-align:baseline;
    background:0 0
}
.main h2,li.last{
    border-bottom:1px solid #888583
}
body{
    line-height:1;
    background:#E4E4E4;
    color:#292929;
    color:rgba(0,0,0,.82);
    font:400 100% Cambria,Georgia,serif;
    -moz-text-shadow:0 1px 0 rgba(255,255,255,.8);
}
ol,ul{
    list-style:none
}
a{
    color:#890101;
    text-decoration:none;
    -moz-transition:.2s color linear;
    -webkit-transition:.2s color linear;
    transition:.2s color linear
}
a:hover{
    color:#DF3030
}
#page{
    padding:0
}
.inner{
    margin:0 auto;
    width:91%
}
.amp{
    font-family:Baskerville,Garamond,Palatino,'Palatino Linotype','Hoefler Text','Times New Roman',serif;
    font-style:italic;
    font-weight:400
}
.mast{
    float:left;
    width:31.875%
}
.title{
    font:semi 700 16px/1.2 Baskerville,Garamond,Palatino,'Palatino Linotype','Hoefler Text','Times New Roman',serif;
    padding-top:0
}
.title h1{
    font:700 20px/1.2 'Book Antiqua','Palatino Linotype',Georgia,serif;
    padding-top:0
}
.author{
    font:400 100% Cambria,Georgia,serif
}
.author i{
    font:400 12px Baskerville,Garamond,Palatino,'Palatino Linotype','Hoefler Text','Times New Roman',serif;
    letter-spacing:.05em;
    padding-top:.7em
}
.footer,.main{
    float:right;
    width:65.9375%
}
ul.nav{
    margin:1em auto 0;
    width:11em
}
ul.nav a{
    font:700 14px/1.2 'Book Antiqua','Palatino Linotype',Georgia,serif;
    letter-spacing:.1em;
    padding:.7em .5em;
    margin-bottom:0;
    text-transform:uppercase
}
input[type=button],input[type=button]:focus{
    background-color:#E4E4E4;
    color:#890101
}
li{
    border-top:1px solid #888583
}
.hide{
    display:none
}
.main h2{
    font-size:1.4em;
    text-align:left;
    margin:0 0 1em;
    padding:0 0 .3em
}
.main{
    position:relative
}
p.left{
    clear:left;
    float:left;
    width:20%;
    min-width:120px;
    max-width:300px;
    margin:0 0 .6em;
    padding:0;
    text-align:right
}
p.right,select{
    min-width:200px
}
p.right{
    overflow:auto;
    margin:0 0 .6em .4em;
    padding-left:.6em;
    text-align:left
}
p.center,p.spacer{
    padding:0;
    display:block
}
.footer,p.center{
    text-align:center
}
p.center{
    float:left;
    clear:both;
    margin:3em 0 3em 15%;
    width:70%
}
p.spacer{
    float:left;
    clear:both;
    margin:0;
    width:100%;
    height:20px
}
input{
    margin:0;
    border:0;
    color:#890101;
    outline:0;
    font:400 100% Cambria,Georgia,serif
}
input[type=text]{
    width:70%;
    min-width:200px;
    padding:0 5px
}
input[type=number]{
    min-width:50px;
    width:50px
}
input:focus{
    background-color:silver;
    color:#000
}
input[type=checkbox]{
    -webkit-appearance:none;
    background-color:#fafafa;
    border:1px solid #cacece;
    box-shadow:0 1px 2px rgba(0,0,0,.05),inset 0 -15px 10px -12px rgba(0,0,0,.05);
    padding:9px;
    border-radius:5px;
    display:inline-block;
    position:relative
}
input[type=checkbox]:active,input[type=checkbox]:checked:active{
    box-shadow:0 1px 2px rgba(0,0,0,.05),inset 0 1px 3px rgba(0,0,0,.1)
}
input[type=checkbox]:checked{
    background-color:#fafafa;
    border:1px solid #adb8c0;
    box-shadow:0 1px 2px rgba(0,0,0,.05),inset 0 -15px 10px -12px rgba(0,0,0,.05),inset 15px 10px -12px rgba(255,255,255,.1);
    color:#99a1a7
}
input[type=checkbox]:checked:after{
    content:'\\2714';
    font-size:14px;
    position:absolute;
    top:0;
    left:3px;
    color:#890101
}
input[type=button],input[type=file]+label{
    font:700 16px/1.2 'Book Antiqua','Palatino Linotype',Georgia,serif;
    margin:17px 0 0
}
input[type=button]{
    position:absolute;
    right:0;
    display:block;
    border:1px solid #adb8c0;
    float:right;
    border-radius:12px;
    padding:5px 20px 2px 23px;
    -webkit-transition-duration:.3s;
    transition-duration:.3s
}
input[type=button]:hover{
    background-color:#909090;
    color:#fff;
    padding:5px 62px 2px 65px
}
input.submit{
    float:left;
    position: relative
}
input.showMessage,input.showMessage:focus,input.showMessage:hover{
    background-color:#6F0;
    color:#000;
    padding:5px 62px 2px 65px
}
input[type=file]{
    width:.1px;
    height:.1px;
    opacity:0;
    overflow:hidden;
    position:absolute;
    z-index:-1
}
input[type=file]+label{
    float:left;
    clear:both;
    cursor:pointer;
    border:1px solid #adb8c0;
    border-radius:12px;
    padding:5px 20px 2px 23px;
    display:inline-block;
    background-color:#E4E4E4;
    color:#890101;
    overflow:hidden;
    -webkit-transition-duration:.3s;
    transition-duration:.3s
}
input[type=file]+label:hover,input[type=file]:focus+label{
    background-color:#909090;
    color:#fff;
    padding:5px 40px 2px 43px
}
input[type=file]+label svg{
    width:1em;
    height:1em;
    vertical-align:middle;
    fill:currentColor;
    margin-top:-.25em;
    margin-right:.25em
}
select{
    margin:0;
    border:0;
    background-color:#DADADA;
    color:#890101;
    outline:0;
    font:400 100% Cambria,Georgia,serif;
    width:50%;
    padding:0 5px
}
.footer{
    border-top:1px solid #888583;
    display:block;
    font-size:12px;
    margin-top:20px;
    padding:.7em 0 20px
}
.footer p{
    margin-bottom:.5em
}
@media (min-width:600px){
    .inner{
        min-width:600px
    }
}
@media (max-width:600px){
    .inner,.page{
        min-width:300px;
        width:100%;
        overflow-x:hidden
    }
    .footer,.main,.mast{
        float:left;
        width:100%
    }
    .mast{
        border-top:1px solid #888583;
        border-bottom:1px solid #888583
    }
    .main{
        margin-top:4px;
        width:98%
    }
    ul.nav{
        margin:0 auto;
        width:100%
    }
    ul.nav li{
        float:left;
        min-width:100px;
        width:33%
    }
    ul.nav a{
        font:12px Helvetica,Arial,sans-serif;
        letter-spacing:0;
        padding:.8em
    }
    .title,.title h1{
        padding:0;
        text-align:center
    }
    ul.nav a:focus,ul.nav a:hover{
        background-position:0 100%
    }
    .author{
        display:none
    }
    .title{
        border-bottom:1px solid #888583;
        width:100%;
        display:block;
        font:400 15px Baskerville,Garamond,Palatino,'Palatino Linotype','Hoefler Text','Times New Roman',serif
    }
    .title h1{
        font:600 15px Baskerville,Garamond,Palatino,'Palatino Linotype','Hoefler Text','Times New Roman',serif;
        display:inline
    }
    p.left,p.right{
        clear:both;
        float:left;
        margin-right:1em
    }
    li,li.first,li.last{
        border:0
    }
    p.left{
        width:100%;
        text-align:left;
        margin-left:.4em;
        font-weight:600
    }
    p.right{
        margin-left:1em;
        width:100%
    }
    p.center{
        margin:1em 0;
        width:100%
    }
    p.spacer{
        display:none
    }
    input[type=text],select{
        width:85%;
    }
    @media (min-width:1300px){
        .page{
            width:1300px
        }
)=====";
