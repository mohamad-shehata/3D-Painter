#include "vtkAutoInit.h" 
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
#include <Mainwindow.h>
#include "ui_MainWindow.h"
#include <vtkSphereSource.h>
#include <vtkCylinderSource.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkCommand.h>
#include <vtkConeSource.h>
#include <vtkCubeSource.h>
#include <vtkDataObjectToTable.h>
#include <vtkElevationFilter.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkQtTableView.h>
#include <vtkVersion.h>
#include <vtkTransform.h>
#include <vtkRendererCollection.h>
#include <vtkActorCollection.h>
#include <QMessageBox>
#include <sstream>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkDataSetMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkProperty.h>
#include <vtkPyramid.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkUnstructuredGrid.h>
MainWindow::MainWindow(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	// Set up the rendering
	this->setWindowTitle("3D Painter");
	mRenderWindow->AddRenderer(mRenderer);
	mRenderWindow->SetInteractor(mInteractor);
	ui->openGLWidget->setRenderWindow(mRenderWindow);
	mInteractor->SetInteractorStyle(mInteractorStyle);
	mInteractor->Initialize();

	// Set the UI connections
	QObject::connect(ui->drawButton, &QPushButton::clicked, this, &MainWindow::onDrawButtonClick);
	QObject::connect(ui->clearButton, &QPushButton::clicked, this, &MainWindow::onClearButtonClick);

	//Fill the color list
	std::stringstream ss(namedColors->GetColorNames());
	std::string color;
	while (std::getline(ss, color, '\n'))
	{
		ui->colorListWidget->addItem(color.c_str());
	}

	// Set opacityLineEdit to accept only numbers from 0 to 99
	ui->opacityLineEdit->setValidator(new QIntValidator(0, 100, this));

	//show message interaction
	std::string info = "1- Right click : \"to Rotate the shape\".\n2- Left click : \"to Scale the shape\".\n3- Scroll wheel button click : \"to Translate the shape\".";
	QMessageBox::information(this, "Information", info.c_str());
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::onClearButtonClick()
{
	while (mRenderer->GetActors()->GetNumberOfItems())
	{
		auto lastActor = mRenderer->GetActors()->GetLastActor();
		mRenderer->RemoveActor(lastActor);
	}
	mRenderWindow->Render();
}

vtkSmartPointer<vtkActor> MainWindow::createShape()
{
	vtkNew<vtkDataSetMapper> shapeMapper;
	// Create shape mapper
	QString type = ui->typeListWidget->selectedItems().first()->text();
	if (type == "Cube") {
		shapeMapper->SetInputConnection(vtkNew<vtkCubeSource>()->GetOutputPort());
	}
	else if (type == "Sphere") {
		vtkNew<vtkSphereSource>sphere;
		sphere->SetThetaResolution(BESTRESOLUTION);
		sphere->SetPhiResolution(BESTRESOLUTION);
		shapeMapper->SetInputConnection(sphere->GetOutputPort());
	}
	else if (type == "Hemisphere") {
		vtkNew<vtkSphereSource>sphere;
		sphere->SetThetaResolution(BESTRESOLUTION);
		sphere->SetPhiResolution(BESTRESOLUTION);
		sphere->SetEndTheta(180);
		shapeMapper->SetInputConnection(sphere->GetOutputPort());
	}
	else if (type == "Cone") {
		vtkNew<vtkConeSource>cone;
		cone->SetResolution(BESTRESOLUTION);
		shapeMapper->SetInputConnection(cone->GetOutputPort());
	}
	else if (type == "Pyramid") {
		vtkNew<vtkConeSource>cone;
		cone->SetResolution(4);
		cone->SetRadius(0.7);
		shapeMapper->SetInputConnection(cone->GetOutputPort());
	}
	else if (type == "Cylinder") {
		vtkNew<vtkCylinderSource> cylinder;
		cylinder->SetResolution(BESTRESOLUTION);
		shapeMapper->SetInputConnection(cylinder->GetOutputPort());
	}
	else if (type == "Tube") {
		vtkNew<vtkCylinderSource> cylinder;
		cylinder->SetResolution(BESTRESOLUTION);
		cylinder->SetCapping(false);
		shapeMapper->SetInputConnection(cylinder->GetOutputPort());
	}
	else {
		QMessageBox::warning(this, "Warning", "The selected shape is undefined.");
		return vtkNew<vtkActor>();
	}

	//shapeMapper->SetInputData(shapeSource->GetOutput());
	shape = vtkNew<vtkActor>();
	shape->SetMapper(shapeMapper);

	// set shape color
	std::string selectedColor = ui->colorListWidget->selectedItems().first()->text().toStdString();
	shape->GetProperty()->SetColor(namedColors->GetColor4d(selectedColor.c_str()).GetData());

	// set shape opacity
	int opacity = ui->opacityLineEdit->text().toInt();
	if (opacity > 100)
	{
		opacity = 100;
		ui->opacityLineEdit->setText(std::to_string(opacity).c_str());
	}
	shape->GetProperty()->SetOpacity(opacity / 100.0);
	return shape;
}

void MainWindow::onDrawButtonClick()
{
	//validate slected color, type, and opacity
	if (ui->typeListWidget->selectedItems().isEmpty() ||
		ui->colorListWidget->selectedItems().isEmpty() ||
		ui->opacityLineEdit->text().isEmpty()
		)
	{
		QMessageBox::warning(this, "Warning", "You have to specify the shape's type, color, and opacity.");
		return;
	}

	// Add the shape actor to the OpenG
	mRenderer->AddActor(createShape());
	mRenderer->ResetCamera();
	mRenderWindow->Render();
}