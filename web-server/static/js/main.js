 // air quality
 function spinner() {
    var classNames = [
      {
        className: 'green',
        title: '空气清新'
      },
      {
        className: 'yellow',
        title: '空气良好'
      },
      {
        className: 'orange',
        title: '轻度污染'
      },
      {
        className: 'purple',
        title: '中度污染'
      },
      {
        className: 'red',
        title: '重度污染'
      }
    ];
    
    var stage = document.getElementById('stage');
    var svg = document.getElementsByClassName('gauge')[0],
        title = svg.getElementsByClassName('gauge_rating')[0];
    	
      stage.className = "gauge " + classNames[i].className;
      title.innerHTML = classNames[i].title; 
 
  };

  spinner();

// light state
  $("#rgb :input").attr("disabled",true);
  var check01 = document.getElementById("check01");
  $("#check01").change(function(){
    if (check01.checked == false) {
      socket.emit('rgb',"0000,0000,0000");
      socket.emit('rgb',"0000,0000,0000");
      $("#rgb :input").attr("disabled",true);
    }else{
      socket.emit('rgb',"1023,1023,1023");
      socket.emit('rgb',"1023,1023,1023");
      $("#rgb :input").attr("disabled",false);
    }
  })

//rgb state
  var colR = document.getElementById("colRed");
  var colG = document.getElementById("colGreen");
  var colB = document.getElementById("colBlue");
  var colPickHTML = document.getElementById("colPick");
  
  colPickHTML.addEventListener("input",changeCol,false);
  
  
  
  function changeCol(e){
    gblRS = (colR.value)*4 + "";
    gblGS = (colG.value)*4 + "";
    gblBS = (colB.value)*4 + "";

    strR = lenTest(gblRS);
    strG = lenTest(gblGS);
    strB = lenTest(gblBS);

    socket.emit('rgb',strR + "," + strG + "," + strB);
  }

  function lenTest(str){
    var newStr = "";
    var len = str.length;
    if(len == 1){
      newStr = "000" + str;
    }else if(len == 2){
      newStr = "00" + str;
    }else if(len == 3){
      newStr = "0" + str;
    }else{
      newStr = str;
    }
    return newStr;
  }

  // rgb model
  var model01 = document.getElementById("model01");
  var model02 = document.getElementById("model02");
  var model03 = document.getElementById("model03");

  model01.addEventListener("click",changeMod01,false);
  model02.addEventListener("click",changeMod02,false);
  model03.addEventListener("click",changeMod03,false);

  function changeMod01(e){
    socket.emit('rgb',"0452,0120,0204");
  }
  function changeMod02(e){
    socket.emit('rgb',"0160,0628,0208");
  }
  function changeMod03(e){
    socket.emit('rgb',"0952,0872,0196");
  }

// 温湿度

//状态
var check02 = document.getElementById("check02");
  $("#check02").change(function(){
    if (check02.checked == false) {
      socket.emit('fan', 0);
    }else{
      socket.emit('fan', 1);
    }
  })

var check03 = document.getElementById("check03");
  $("#check03").change(function(){
    if (check03.checked == false) {
      socket.emit('curtain', 0);
    }else{
      socket.emit('curtain', 1);
    }
  })
// 警报
if(bellState == 1){
  document.getElementById("bell").className += " hbell";
}

// 当前时间
function check(val){ 
 if (val<10){ 
  return("0"+val); 
 }else{ 
  return(val); 
 } 
} 
function ShowTime() 
{ 
 var date=new Date(); 
 var year=date.getFullYear(); 
 var month=date.getMonth()+1; 
 var day=date.getDate(); 
 var hour=date.getHours(); 
 var minutes=date.getMinutes(); 
 var second=date.getSeconds(); 
 var timestr=year+"年"+month+"月"+day+"日  "+check(hour)+":"+check(minutes)+":"+check(second); 
 document.getElementById("time").innerText=timestr; 
 var timerID=setTimeout('ShowTime()',1000); 
} 
ShowTime();
