// TestView.cpp : implementation of the CTestView class
//

#include "stdafx.h"
#include "Test.h"
#include "TestDoc.h"
#include "TestView.h"
#include "math.h"
#define Round(d) int(floor(d+0.5))//四舍五入宏定义
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTestView

IMPLEMENT_DYNCREATE(CTestView, CView)

BEGIN_MESSAGE_MAP(CTestView, CView)
	//{{AFX_MSG_MAP(CTestView)
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(IDM_DRAWPIC, OnDrawpic)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestView construction/destruction

CTestView::CTestView()
{
	// TODO: add construction code here	
	bFill=FALSE;
	SeedClr=RGB(255,0,0);
}

CTestView::~CTestView()
{
}

BOOL CTestView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CTestView drawing

void CTestView::OnDraw(CDC* pDC)
{
	CTestDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
	DrawGraph(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CTestView printing

BOOL CTestView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CTestView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CTestView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CTestView diagnostics

#ifdef _DEBUG
void CTestView::AssertValid() const
{
	CView::AssertValid();
}

void CTestView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTestDoc* CTestView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTestDoc)));
	return (CTestDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTestView message handlers

void CTestView::DrawGraph(CDC *pDC)//绘制图形
{
	GetClientRect(&rect);
	pDC->SetMapMode(MM_ANISOTROPIC);//显示DC自定义坐标系
	pDC->SetWindowExt(rect.Width(),rect.Height());
	pDC->SetViewportExt(rect.Width(),-rect.Height());
	pDC->SetViewportOrg(rect.Width()/2,rect.Height()/2);
	CDC memDC;//声明一个内存DC
	memDC.CreateCompatibleDC(pDC);//创建一个与显示DC兼容的内存DC
	CBitmap NewBitmap,*pOldBitmap; 
	NewBitmap.LoadBitmap(IDB_BITMAP2);//从资源中导入空心汉字的位图
	BITMAP bmpInfo; //声明bmpInfo结构体
	NewBitmap.GetBitmap(&bmpInfo);//获取位图信息
	pOldBitmap=memDC.SelectObject(&NewBitmap);//将位图选入内存DC
	memDC.SetMapMode(MM_ANISOTROPIC);//内存DC自定义坐标系
	memDC.SetWindowExt(bmpInfo.bmWidth,bmpInfo.bmHeight);
	memDC.SetViewportExt(bmpInfo.bmWidth,-bmpInfo.bmHeight);
	memDC.SetViewportOrg(bmpInfo.bmWidth/2,bmpInfo.bmHeight/2);
	rect.OffsetRect(-rect.Width()/2,-rect.Height()/2);
	int nX=rect.left+(rect.Width()-bmpInfo.bmWidth)/2;//计算位图在客户区的中心点
    int nY=rect.top+(rect.Height()-bmpInfo.bmHeight)/2;
	pDC->BitBlt(nX,nY,rect.Width(),rect.Height(),&memDC,-bmpInfo.bmWidth/2,-bmpInfo.bmHeight/2,SRCCOPY);//将内存DC中的位图拷贝到设备DC
	if(bFill)
		CharFill(pDC);//填充空心汉字
	memDC.SelectObject(pOldBitmap);//从内存DC中释放位图
	memDC.DeleteDC();//删除memDC
}

void CTestView::CharFill(CDC *pDC)//文字填充函数
{
	COLORREF BoundaryClr=RGB(0,0,0);//边界色
	BOOL bSpanFill;
	pHead=new CStackNode;//建立栈结点
	pHead->pNext=NULL;//栈头结点的指针域总为空
	Push(Seed);//种子像素入栈
	int x,y,x0=Round(Seed.x),y0=Round(Seed.y);//x，y用于判断种子与图形的位置关系
	x=x0-1;
	while(pDC->GetPixel(x,y0)!=BoundaryClr && pDC->GetPixel(x,y0)!=SeedClr)//左方判断
	{
		x--;
		if(x<=-rect.Width()/2)
		{
			MessageBox("种子不在图形之内","警告");//到达客户区最左端
			return;
		}
	}
	y=y0+1;
	while(pDC->GetPixel(x0,y)!=BoundaryClr && pDC->GetPixel(x0,y)!=SeedClr)//上方判断
	{
		y++;
		if(y>=rect.Height()/2)//到达客户区最上端
		{
			MessageBox("种子不在图形之内","警告");
			return;
		}
	}
	x=x0+1;
	while(pDC->GetPixel(x,y0)!=BoundaryClr && pDC->GetPixel(x,y0)!=SeedClr)//右方判断
	{
		x++;
		if(x>=rect.Width()/2)//到达客户区最右端
		{
			MessageBox("种子不在图形之内","警告");		
			return;
		}
	}
	y=y0-1;
	while(pDC->GetPixel(x0,y)!=BoundaryClr && pDC->GetPixel(x0,y)!=SeedClr)//下方判断
	{
		y--;
		if(y<=-rect.Height()/2)//到达客户区最下端
		{
			MessageBox("种子不在图形之内","警告");
			return;
		}
	}
	double xleft,xright;//区间最左端与最右端像素
	CP2 PopPoint,PointTemp;
	while(pHead->pNext!=NULL)//如果栈不为空
	{
		Pop(PopPoint);
		if(pDC->GetPixel(Round(PopPoint.x),Round(PopPoint.y))==SeedClr)
			continue;
		PointTemp=PopPoint;
		while(pDC->GetPixel(Round(PointTemp.x),Round(PointTemp.y))!=BoundaryClr && pDC->GetPixel(Round(PointTemp.x),Round(PointTemp.y))!=SeedClr)
		{
			pDC->SetPixelV(Round(PointTemp.x),Round(PointTemp.y),SeedClr);
			PointTemp.x++;
		}
		xright=PointTemp.x-1;
		PointTemp.x=PopPoint.x-1;
		while(pDC->GetPixel(Round(PointTemp.x),Round(PointTemp.y))!=BoundaryClr && pDC->GetPixel(Round(PointTemp.x),Round(PointTemp.y))!=SeedClr)
		{
			pDC->SetPixelV(Round(PointTemp.x),Round(PointTemp.y),SeedClr);
			PointTemp.x--;
		}
		xleft=PointTemp.x+1;
		//处理上一条扫描线
		PointTemp.x=xleft;
		PointTemp.y=PointTemp.y+1;
		while(PointTemp.x<xright)
		{
			bSpanFill=FALSE;
			while(pDC->GetPixel(Round(PointTemp.x),Round(PointTemp.y))!=BoundaryClr && pDC->GetPixel(Round(PointTemp.x),Round(PointTemp.y))!=SeedClr)
			{
				bSpanFill=TRUE;
				PointTemp.x++;
			}
			if(bSpanFill)
			{
				if(PointTemp.x==xright && pDC->GetPixel(Round(PointTemp.x),Round(PointTemp.y))!=BoundaryClr && pDC->GetPixel(Round(PointTemp.x),Round(PointTemp.y))!=SeedClr)
					PopPoint=PointTemp;
				else
				{
					PopPoint.x=PointTemp.x-1;
					PopPoint.y=PointTemp.y;
				}
				Push(PopPoint);
				bSpanFill=FALSE;
			}
			while((pDC->GetPixel(Round(PointTemp.x),Round(PointTemp.y))==BoundaryClr && PointTemp.x<xright) || (pDC->GetPixel(Round(PointTemp.x),Round(PointTemp.y))==SeedClr && PointTemp.x<xright))
			    PointTemp.x++;
		}
		//处理下一条扫描线
		PointTemp.x=xleft;
		PointTemp.y=PointTemp.y-2;
		while(PointTemp.x<xright)
		{
			bSpanFill=FALSE;
			while(pDC->GetPixel(Round(PointTemp.x),Round(PointTemp.y))!=BoundaryClr && pDC->GetPixel(Round(PointTemp.x),Round(PointTemp.y))!=SeedClr)
			{
				bSpanFill=TRUE;
				PointTemp.x++;
			}
			if(bSpanFill)
			{
				if(PointTemp.x==xright && pDC->GetPixel(Round(PointTemp.x),Round(PointTemp.y))!=BoundaryClr && pDC->GetPixel(Round(PointTemp.x),Round(PointTemp.y))!=SeedClr)
					PopPoint=PointTemp;
				else
				{
					PopPoint.x=PointTemp.x-1;
					PopPoint.y=PointTemp.y;
				}
				Push(PopPoint);
				bSpanFill=FALSE;
			}
			while((pDC->GetPixel(Round(PointTemp.x),Round(PointTemp.y))==BoundaryClr && PointTemp.x<xright) || (pDC->GetPixel(Round(PointTemp.x),Round(PointTemp.y))==SeedClr && PointTemp.x<xright))
			    PointTemp.x++;
		}
	}
	delete pHead;
	pHead=NULL;
}

void CTestView::Push(CP2 point)//入栈函数
{
	pTop=new CStackNode;
	pTop->PixelPoint=point;
	pTop->pNext=pHead->pNext;
	pHead->pNext=pTop;
}

void CTestView::Pop(CP2 &point)//出栈函数
{
	if(pHead->pNext!=NULL)
	{
		pTop=pHead->pNext;
		pHead->pNext=pTop->pNext;
		point=pTop->PixelPoint;
		delete pTop;
	}
}

void CTestView::OnLButtonDown(UINT nFlags, CPoint point)//左键按下函数 
{
	// TODO: Add your message handler code here and/or call default
	Seed=CP2(point.x-rect.Width()/2,rect.Height()/2-point.y);//选择种子位置
	Invalidate(FALSE);
	CView::OnLButtonDown(nFlags, point);
}

void CTestView::OnDrawpic() 
{
	// TODO: Add your command handler code here
	CColorDialog ccd(SeedClr,CC_SOLIDCOLOR);
	if(IDOK==ccd.DoModal())//调用颜色对话框选取填充色
		SeedClr=ccd.GetColor();	
	else
		return;	
	if(IDOK==MessageBox("请在空心字体内单击鼠标左键!","提示",MB_OKCANCEL))
	    bFill=TRUE;
	else
		return;	
}
