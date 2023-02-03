#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <QVTKInteractor.h>
#include <vtkInteractorStyle.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkNamedColors.h>
#include <vtkUnstructuredGrid.h>

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget* parent = nullptr);
	~MainWindow();
public slots:
	void onDrawButtonClick();
	void onClearButtonClick();
private:
	const int BESTRESOLUTION = 100;
	vtkNew<vtkNamedColors> namedColors;
	vtkSmartPointer<vtkActor> shape;
	Ui::MainWindow* ui;
	vtkNew<vtkGenericOpenGLRenderWindow> mRenderWindow;
	vtkNew<vtkRenderer> mRenderer;
	vtkNew<QVTKInteractor> mInteractor;
	vtkNew<vtkInteractorStyleTrackballActor> mInteractorStyle;
	vtkSmartPointer<vtkActor> createShape();
};

#endif // MAINWINDOW_H
