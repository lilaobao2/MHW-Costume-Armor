#include "MainWindow.hpp"
#include "Config.h"
#include <QThread>
#include <QMovie>
#include <QInputDialog>
#include <QFileDialog>
#include <QActionGroup>
#include <QMessageBox>

/// These file contains Member definitions of the MainWindow class
/// Related to The managment of input Comboboxes

void MainWindow::_translateArmorData()
{
    QAction *checked = _langGroup->checkedAction();
    _settings.setValue("General/Language", checked->text());
    auto Lang = checked->text().toStdString();

    QFont font;
    for (auto lAction : _langActions)
        lAction->setFont(font);

    font.setBold(true);
    checked->setFont(font);

    _transArmorData.clear();

    for(const auto &el : _ArmorData.items())
    {
        _transArmorData[el.value()[Lang].get<std::string>()] = el.value();
        _transArmorData[el.value()[Lang].get<std::string>()]["ID"] = std::stoi(el.key());
    }
    for (int i = 0; i < 5; ++i)
    {
        this->_inputBoxes[i]->clear();
    }
    _populateComboBoxes();
    this->_settings.sync();
}

void MainWindow::_updateArmorValues()
{
    auto Data = _MHManager.getPlayerData();
    int index;
    for (int i = 0; i < 5; ++i)
    {
        index = _inputBoxes[i]->findData((int)Data[i]);
        if (index < 0)
        {
            index = 0;
            LOG_ENTRY(WARNING, "Encountered unknown value (" << (int)Data[i] << ") changing it to "
                     << Armor::NOTHING << " for safety");
        }
        _inputBoxes[i]->setCurrentIndex(index);
    }
}

void MainWindow::_clearArmor()
{
    for(int i=0; i<5 ;++i)
        _MHManager.getPlayerData()[i] = Armor::NOTHING;
    this->_updateArmorValues();
}

bool MainWindow::_parseInputBoxes()
{
    int Val;
    for (int i = 0; i < 5; ++i)
    {
        Val = _inputBoxes[i]->currentData().toInt();
        if (Val == 0) // Means Error btw
        {
            DialogWindow *Dia = new DialogWindow(this, "ERROR", "Invalid Value for armor", Status::ERROR0);
            Dia->show();
            this->_fetchData(true);
            return false;
        }
        this->_MHManager.getPlayerData()[i] = Val;
    }
    return true;
}

void MainWindow::_changeAll()
{
    QStringList items;
    int i = 0;
    for (auto &it : _transArmorData.items())
        if (it.value()["Mode"] == "11111") // Means its a set
            items << it.key().c_str();

    bool ok;
    QString text = getItemInputDialog(this, "Change All Armor", "Select set: ", items, &ok);
    if (!ok)
        return;
    json Selected = _transArmorData[text.toStdString()];
    for (i = 0; i < 5; ++i)
        _MHManager.getPlayerData()[i] = Selected["ID"];

    this->_updateArmorValues();
}

void MainWindow::_manualInputValue()
{
    bool ok;
    int id = QInputDialog::getInt(this, "Manually Input ID",
                                 "Input Armor ID", Armor::NOTHING, 1, Armor::NOTHING, 1, &ok);
    if (!ok)
        return;
    for (int i = 0; i < 5; ++i)
    {
        this->_inputBoxes[i]->insertItem(0, QString("Custom ID : %1").arg(id), id);
        this->_inputBoxes[i]->setCurrentIndex(0);
    }
}