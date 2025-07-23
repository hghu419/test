//  *: This code is going to draw variable with simple lhcb style, it can set range of hist automatically.
//  *: Before you running this code, please compile the code first.
//    command: g++ -o draw.out draw.cpp $(root-config --libs --cflags)
//    usage: ./draw.out <InputRootFile> <TreeName> <VariableName> [options]

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TString.h"
#include "TApplication.h"

void DrawVariable(const char *rootFileName, const char *treeName, const char *variableName,
                  double xMin = 0, double xMax = 0, bool autoRange = true,
                  const char *outputFileName = nullptr)
{
  gROOT->ProcessLine(".x /DATA/xcsong/lib/lhcbStyle.C");
  // 打开ROOT文件
  TFile *file = TFile::Open(rootFileName);
  if (!file || file->IsZombie())
  {
    std::cerr << "Error: cannot open file " << rootFileName << std::endl;
    return;
  }

  // 获取TTree
  TTree *tree = (TTree *)file->Get(treeName);
  if (!tree)
  {
    std::cerr << "Error: cannot find tree " << treeName << " in file " << rootFileName << std::endl;
    file->Close();
    return;
  }

  // 创建画布
  TCanvas *c1 = new TCanvas("c1", "Canvas", 800, 600);
  c1->cd();

  // 自动设置X轴范围
  if (autoRange)
  {
    tree->Draw(Form("%s>>htemp", variableName));
    TH1 *h1 = (TH1 *)gPad->GetPrimitive("htemp");
    if (h1)
    {
      xMin = h1->GetXaxis()->GetXmin();
      xMax = h1->GetXaxis()->GetXmax();
      std::cout << "Auto range: X axis set to [" << xMin << ", " << xMax << "]" << std::endl;
    }
    else
    {
      std::cerr << "Error: failed to get histogram for " << variableName << std::endl;
      file->Close();
      return;
    }
  }

  // 绘制变量
  TString drawCmd = Form("%s>>hvar", variableName);
  tree->Draw(drawCmd);

  // 获取直方图并设置属性
  TH1 *hvar = (TH1 *)gPad->GetPrimitive("hvar");
  if (hvar)
  {
    // 设置X轴范围
    if (!autoRange || (xMin != 0 || xMax != 0))
    {
      hvar->GetXaxis()->SetRangeUser(xMin, xMax);
    }

    hvar->SetTitle(Form("Distribution of %s", variableName));
    hvar->GetXaxis()->SetTitle(variableName);
    hvar->GetYaxis()->SetTitle("Entries");

    // 布局调整
    gPad->SetLeftMargin(0.15);
    hvar->GetYaxis()->SetTitleOffset(1.1);
    c1->Update();

    // 确定输出文件名
    TString outFileName;
    if (outputFileName)
    {
      outFileName = outputFileName;
    }
    else
    {
      outFileName = Form("%s_distribution.png", variableName);
    }

    // 保存图像
    c1->SaveAs(outFileName);
    std::cout << "Plot saved as " << outFileName << std::endl;
  }
  else
  {
    std::cerr << "Error: unable to draw variable " << variableName << std::endl;
  }

  // 清理资源
  file->Close();
  delete c1;
}

/*
void draw(const char* rootFileName, const char* treeName, const char* variableName,const char* outFileName){
    DrawVariable(rootFileName, treeName, variableName, 0, 0, true, outFileName);
}
*/

int main(int argc, char **argv)
{
  // 参数解析
  if (argc < 4)
  {
    std::cerr << "Usage: " << argv[0] << " <InputRootFile> <TreeName> <VariableName> [options]\n"
              << "Options:\n"
              << "  -o <output.png>   Output file name\n"
              << "  -r <min> <max>    Manual axis range\n"
              << "  -a                Enable auto range (default)\n"
              << "  -m                Disable auto range (manual mode)"
              << std::endl;
    return 1;
  }
  // 解析位置参数

  const char *rootFileName = argv[1];
  const char *treeName = argv[2];
  const char *variableName = argv[3];

  // 处理可选参数
  const char *outputFile = nullptr;
  double xMin = 0, xMax = 0;
  bool autoRange = true;
  bool hasRange = false;

  for (int i = 4; i < argc;)
  {
    std::string arg = argv[i];

    if (arg == "-o" && i + 1 < argc)
    {
      outputFile = argv[i + 1];
      i += 2;
    }
    else if (arg == "-r" && i + 2 < argc)
    {
      xMin = atof(argv[i + 1]);
      xMax = atof(argv[i + 2]);
      autoRange = false;
      hasRange = true;
      i += 3;
    }
    else if (arg == "-a")
    {
      autoRange = true;
      i++;
    }
    else if (arg == "-m")
    {
      autoRange = false;
      i++;
    }
    else
    {
      std::cerr << "Unknown option or missing arguments: " << arg << std::endl;
      return 1;
    }
  }

  // 验证范围参数
  if (!autoRange && !hasRange)
  {
    std::cerr << "Manual range requested but no range provided. Use -r <min> <max>"
              << std::endl;
    return 1;
  }

  // 调用绘图函数
  DrawVariable(rootFileName, treeName, variableName, xMin, xMax, autoRange, outputFile);

  return 0;
}
