var root = GetByName("window_main");//取根对象
var DesignTabHead = GetByName("designtabhead");//取设计区选择夹头部对象
var DesignTab = GetByName("designtab");//取设计区选择夹对象


GetByName("ctl").WindowCtl_clos.onclick=function (c){exit();};//设置关闭按钮回调

//------------设计区
var DesignTabSelectIndex=0;//当前选择的选择夹
function DesignTabSelect(Index){
	var old = DesignTabHead.GetItemAt(DesignTabSelectIndex-1);
	if(old){
		old.bkcolor=0;
		old.closbut.visible=false;
		old.tit.padding="0,0,11,0";
	}
	var c = DesignTabHead.GetItemAt(Index-1);
	if(c){
		c.bkcolor=0xFFFFF29D;
		c.closbut.visible=true;
		c.tit.padding="0,0,0,0";
	}
	DesignTabSelectIndex=Index;
	DesignTab.selectitem=Index-1;
}
function AddDesignTab(type){
	var TabHead = new Control("HorizontalLayout");
	TabHead.height=22;
	TabHead.minwidth=60;
	TabHead.maxwidth=170;
	TabHead.onmouseenter=function(c){
		if(DesignTabHead.GetItemIndex(c)+1!=DesignTabSelectIndex){
			c.bkcolor=0xFF5B7199;
			c.closbut.visible=true;
			c.tit.padding="0,0,0,0";
		}
	};
	TabHead.onmouseleave=function(c){
		if(DesignTabHead.GetItemIndex(c)+1!=DesignTabSelectIndex){
			c.bkcolor=0;
			c.closbut.visible=false;
			c.tit.padding="0,0,11,0";
		}	
	};
	TabHead.onclick=function(c){
		var Index=DesignTabHead.GetItemIndex(c)+1;
		if(Index!=DesignTabSelectIndex){
			DesignTabSelect(Index);
		}
	};
	

	var title = new Control("Label");
	title.name="tit";//设置名字,方便直接以成员方式操作
	title.text="a.xml"
	TabHead.Add(title);
	var clos = new Control("Button");
	clos.width=16;
	clos.height=16;
	clos.padding="3,3,3,3";
	clos.normalimage="res:img/clos.png:type='img':src='0,0,18,18'" 
	clos.hotimage="res:img/clos.png:type='img':src='0,18,18,18'" 
	clos.pushedimage="res:img/clos.png:type='img':src='0,36,18,18'"
	clos.name="closbut";
	clos.onclick=function(c){
		var a=c.parent;
		var i =DesignTabHead.GetItemIndex(a);
		c.parent.clos();
		DesignTab.GetItemAt(i).clos();
		if (DesignTab.count == 0) {
		    Design.GetItemAt(1).bkcolor = 0xFF293A56;
		    Design.GetItemAt(2).bkcolor = 0xFF293A56;
		}
		if(i+1<=DesignTabSelectIndex)
			DesignTabSelectIndex--;
		DesignTabSelect(DesignTabSelectIndex);
	}//关闭标签
	clos.visible=false;
	TabHead.Add(clos);
	Design.GetItemAt(1).bkcolor = 0xFFFFF29D;
	Design.GetItemAt(2).bkcolor = 0xFFBDC5D8;

	DesignTabHead.AddAt(TabHead);//添加到父控件
	
	if(type=="js"){
		var Edit = new Control("CodeEdit");
		DesignTab.Add(Edit);
	}else if(type=="window"){
		var Edit = new Control("CodeEdit");
		DesignTab.Add(Edit);
	}else if(type=="xml"){
		var Edit = new Control("CodeEdit");
		DesignTab.Add(Edit);
	}
	DesignTabSelect(DesignTab.count);
	return Edit;
}


//------------属性区
var AttTabSelectIndex=1;//当前选择的选择夹
//挂接事件
var atttabhead=GetByName("atttabhead");
var atttab=GetByName("atttab");
atttabhead.工程.onmouseenter=atttabheadonmouseenter;
atttabhead.工程.onmouseleave=atttabheadonmouseleave;
atttabhead.工程.onclick=atttabheadonclick;
atttabhead.属性.onmouseenter=atttabheadonmouseenter;
atttabhead.属性.onmouseleave=atttabheadonmouseleave;
atttabhead.属性.onclick=atttabheadonclick;
function atttabheadonmouseenter(c){
	if(atttabhead.GetItemIndex(c)+1!=AttTabSelectIndex){
		c.bkcolor=0xFF5B7199;
	}
};
function atttabheadonmouseleave(c){
	if(atttabhead.GetItemIndex(c)+1!=AttTabSelectIndex){
		c.bkcolor=0;
	}	
};
function atttabheadonclick(c){
	var Index=atttabhead.GetItemIndex(c)+1;
	if(Index!=AttTabSelectIndex){
		AttTabSelect(Index);
	}
};
function AttTabSelect(Index){
	var old = atttabhead.GetItemAt(AttTabSelectIndex-1);
	if(old){
		old.bkcolor=0;
	}
	var c = atttabhead.GetItemAt(Index-1);
	if(c){
		c.bkcolor=0xFFFFFFFF;
	}
	AttTabSelectIndex=Index;
	atttab.selectitem=Index-1;
}


//------------工具区



//------------初始化代码
AddDesignTab("window");
AddDesignTab("xml");




