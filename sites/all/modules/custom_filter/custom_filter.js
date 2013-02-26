var CustomFilter = {};

if(Drupal.jsEnabled) {
   $(document).ready(function(){
      $("#show-type .condition-type, #show-location .condition-location").hover(
	   function() {
	        $(this).find('span').addClass('tmpOver');
      	   } ,
	   function() {
        	$(this).find('span').removeClass('tmpOver');
           }
      );/*show-type hover END*/

     $("#show-type .condition-type, #show-location .condition-location").click(
	  function() {
		var locationID = "all";
		var typeID = "all";
		var tmpNum;
		//$(this).parent().parent().parent().parent().find('.filter-condition dl dd li span').removeClass('tmpClick');
	        $(this).find('span').addClass('tmpClick');
	        $(this).siblings().find('span').removeClass('tmpClick');
		$(this).parent().siblings().children().find('span').removeClass('tmpClick');
		var tmpCondition = $(this).parent().parent().attr("id");

		if(tmpCondition == "show-type"){
		     var tmpNum =  $(this).attr("id").substring(5,6);
		     var typeID = parseInt(tmpNum);
		     tmpNum = $(this).parent().parent().parent().parent().find('.filter-condition #show-location dd li .tmpClick').parent().attr("id")
		     if (tmpNum){
		     	locationID = parseInt(tmpNum.substring(9,10));
		    } 
		}
		else if(tmpCondition == "show-location"){
		     var tmpNum =  $(this).attr("id").substring(9,10);
		     var locationID = parseInt(tmpNum);
		     tmpNum = $(this).parent().parent().parent().parent().find('.filter-condition #show-type dd li .tmpClick').parent().attr("id");
		    if(tmpNum){
		    	typeID = parseInt(tmpNum.substring(5,6));
		    }
		}

		var tmpShow = "Location：" + locationID + "； Type：" + typeID;

		$.post(Drupal.settings.custom_filter.post_url , { "type_id":typeID ,  "location_id":locationID } ,function(data) {
           		var $resultData = Drupal.parseJson(data);
	   		result ="";
	   		for( var $i = 0 ; $i < $resultData.num; $i++ ){
			     $k = $i%4 +1;
			     $result_num = "shtg-"+$k ;
	      		     tmp =  "<li class='type-title "+$result_num+"'><a class='author-title' href='"+$resultData['url'][$i]+"'><span class='lan'>["+$resultData['type'][$i]+"]</span>"+$resultData['title'][$i]+"</a></li>";
	      		     result = result+tmp;
	        	}
			result = "<ul class='result-1-1'>"+result+"</ul>";
                       $("#show-text").html(result);
              });	

      	  } 
     );

  });/*document END*/


}



