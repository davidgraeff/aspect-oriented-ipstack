#ifndef PICKDEPENDENCIES_H
#define PICKDEPENDENCIES_H

#include <QDialog>
#include <QStringList>

namespace Ui {
class PickDependencies;
}

class PickDependencies : public QDialog
{
    Q_OBJECT
    
public:
    explicit PickDependencies(const QString& kconfig_input_filename, QWidget *parent = 0);
    ~PickDependencies();

    QStringList list_of_selected_features;
    
private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::PickDependencies *ui;
};

#endif // PICKDEPENDENCIES_H
