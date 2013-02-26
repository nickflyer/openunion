var CustomFilter = {};
if(Drupal.jsEnabled) {
    var sWidth = 149;//单个容器宽度(包括边距，填充),PS:每次位移距离
    var visible = 4;//显示数量
    var mr = 0; //向左移动的图片个数（为负值）
    var listLength ;//列表图片个数
    var listWidth ;//列表宽度
    var listLeft ;//列表位置
    var bId = $("#scrollingImages");//大容器ID
    var bWidth = bId.width(); ;//容器宽度
    var listId;//列表ID
    var trendLeft;//变化侧栏值
    var maxMr;
   $(document).ready(function(){
	listId = $("#Cont");
	listLength = listId.find("li").length;
	listWidth = listLength*sWidth;
	listLeft =parseInt(listId.css('left'));
	maxMr = listLength - visible;
      $("#block-openunion_images-0  #LeftBotton").click(
	    function(){	
		if(listWidth>bWidth){
		    if(mr < 0){
			mr++;
			trendLeft = mr*sWidth;
			listId.animate({
			    left:trendLeft + "px"
			},200);
		    }		
	        }/*if 结束*/
	   }
     );
     $("#block-openunion_images-0  #RightBotton").click(
	    function(){
		if(listWidth>bWidth){
		   if(-mr < maxMr){
		       mr--;//每次移动的个数如果我们要移动7个，则为 mr = mr-7;
		       trendLeft = mr*sWidth;
		       listId.animate({
			  left:trendLeft + "px"
		       },200);
		   }	
	        }/*if 结束*/
	   }
     );/*RightBotton end*/

  });/*document END*/
}
