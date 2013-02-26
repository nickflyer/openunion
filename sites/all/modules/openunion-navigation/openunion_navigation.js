var CustomFilter = {};
if(Drupal.jsEnabled) {
   $(document).ready(function(){
      $("#navigation-title .menu-title li").hover(
	   function() {
	        $(this).find('a').addClass('tmpOver');
      	   } ,
	   function() {
        	$(this).find('a').removeClass('tmpOver');
           }
      );/*show-type hover END*/
     $("#navigation-title .menu-title li").click(
	  function() {
	        $(this).find('a').addClass('tmpClick');
	        $(this).siblings().find('a').removeClass('tmpClick');

		var showType = $(this).attr("id").substring(0,1);

		$.post(Drupal.settings.openunion_navigation.post_url , { "show_type":showType } ,function(data) {
           		var $resultData = Drupal.parseJson(data);
	   		result ="";
	   		for( var $i = 0 ; $i < $resultData.num; $i++ ){
			     $k = $i +1;
	      		     tmp =  "<li class='type-title'><a class='author-title' href='"+$resultData['url'][$i]+"'>"+$resultData['title'][$i]+"</a></li>";
	      		     result = result+tmp;
	        	}
			result = "<ul class='ul-result'>"+result+"</ul>";
                       $("#navigation-result .show-text").html(result);
              });	
      	  } 
     );
  });/*document END*/
}
