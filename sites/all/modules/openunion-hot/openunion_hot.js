var CustomFilter = {};
if(Drupal.jsEnabled) {
   $(document).ready(function(){
      $("#volti-title .menu-title li").hover(
	   function() {
	        $(this).find('span').addClass('tmpOver');
      	   } ,
	   function() {
        	$(this).find('span').removeClass('tmpOver');
           }
      );/*show-type hover END*/
     $("#volti-title .menu-title li").click(
	  function() {
		//$(this).parent().parent().parent().parent().find('.filter-condition dl dd li span').removeClass('tmpClick');
	        $(this).find('span').addClass('tmpClick');
	        $(this).siblings().find('span').removeClass('tmpClick');
		var showType = $(this).attr("id").substring(0,1);
//		var tmpShow = "Location：" + locationID + "； Type：" + typeID;
		$.post(Drupal.settings.openunion_hot.post_url , { "show_type":showType } ,function(data) {
           		var $resultData = Drupal.parseJson(data);
	   		result ="";
	   		for( var $i = 0 ; $i < $resultData.num; $i++ ){
			     $k = $i +1;
	      		     tmp =  "<li class='type-title'><div class='px'>"+$k+"</div><a class='author-title' href='"+$resultData['url'][$i]+"'>"+$resultData['title'][$i]+"</a><span class='score'>"+$resultData['score'][$i]+"</span></li>";
	      		     result = result+tmp;
	        	}
			result = "<ul class='ul-result'>"+result+"</ul>";
                       $("#volti-result .show-text").html(result);
              });	
      	  } 
     );
  });/*document END*/


}



