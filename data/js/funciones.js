function onchangech(ch){
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function(){
    if (this.readyState == 4 && this.status == 200){
      if(ch=='11'){
       document.getElementById('stc1').innerHTML = 'Encendido';
      }
     if(ch=='10'){
       document.getElementById('stc1').innerHTML = 'Apagado';
      }
     if(ch=='21'){
       document.getElementById('stc2').innerHTML = 'Encendido';
      }
     if(ch=='20'){
       document.getElementById('stc2').innerHTML = 'Apagado';
      }
     }
    };
  xhttp.open('GET', 'canal?accion='+ch, true);
  xhttp.send();
}
function refresh(){
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function(){
    if (this.readyState == 4 && this.status == 200){
      state=this.responseText.split(',');
      if (state[0]=='1'){document.getElementById('stc1').innerHTML='Encendido';}
      if (state[0]=='0'){document.getElementById('stc1').innerHTML='Apagado';}
      if (state[1]=='1'){document.getElementById('stc2').innerHTML='Encendido';}
      if (state[1]=='0'){document.getElementById('stc2').innerHTML='Apagado';}
      document.getElementById('ti').innerHTML=state[2];
      document.getElementById('hi').innerHTML=state[3];
      document.getElementById('tp').innerHTML=state[4];
      document.getElementById('hp').innerHTML=state[5];
    }
  };
  xhttp.open('GET', 'refresh', true);
  xhttp.send();
}
function valida(par){
  switch (par){
   case 0:
    $('#contenedor').load('estado.html');
    break;
   case 1:
    document.forms['actualiza'].submit();
    break;
  }
}