﻿
// UltrasoundSimulationView.h: CUltrasoundSimulationView 类的接口
//

#pragma once

#include "UltrasoundSimulationDoc.h"
class CUltrasoundSimulationView : public CView
{
protected: // 仅从序列化创建
	CUltrasoundSimulationView() noexcept;
	DECLARE_DYNCREATE(CUltrasoundSimulationView)

// 特性
public:
	CUltrasoundSimulationDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 实现
public:
	virtual ~CUltrasoundSimulationView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // UltrasoundSimulationView.cpp 中的调试版本
inline CUltrasoundSimulationDoc* CUltrasoundSimulationView::GetDocument() const
   { return reinterpret_cast<CUltrasoundSimulationDoc*>(m_pDocument); }
#endif

