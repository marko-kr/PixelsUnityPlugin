#pragma once

#include "BleCommon.h"
#include "DiscoveredPeripheral.h"

namespace Systemic::BluetoothLE
{
    class DiscoveredPeripheral;

    class Scanner final
    {
        using BluetoothLEAdvertisementWatcher = winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher;
        using BluetoothLEAdvertisementReceivedEventArgs = winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementReceivedEventArgs;
        using BluetoothLEAdvertisementWatcherStoppedEventArgs = winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcherStoppedEventArgs;

        BluetoothLEAdvertisementWatcher _watcher{ nullptr };
        std::vector<winrt::guid> _requestedServices{};
        std::map<bluetooth_address_t, std::shared_ptr<DiscoveredPeripheral>> _peripherals{};
        std::function<void(std::shared_ptr<DiscoveredPeripheral>)> _onPeripheralDiscovered{};
        winrt::event_token _receivedToken{};
        winrt::event_token _stoppedToken{};

    public:
        Scanner(
            std::function<void(std::shared_ptr<DiscoveredPeripheral>)> peripheralDiscovered,
            std::vector<winrt::guid> services = std::vector<winrt::guid>{},
            std::function<void()> scanStopped = nullptr)
            :
            _watcher{},
            _requestedServices{ services },
            _onPeripheralDiscovered{ peripheralDiscovered }
        {
            using namespace winrt::Windows::Devices::Bluetooth::Advertisement;

            _watcher.AllowExtendedAdvertisements(true);
            _watcher.ScanningMode(BluetoothLEScanningMode::Active);

            _receivedToken = _watcher.Received({ this, &Scanner::onReceived });
            _stoppedToken = _watcher.Stopped({ this, &Scanner::onStopped });

            _watcher.Start();
        }

        ~Scanner()
        {
            if (_watcher)
            {
                _watcher.Received(_receivedToken);
                _watcher.Stopped(_stoppedToken);
                _watcher.Stop();
                _watcher = nullptr;
            }
        }

    private:
        void onReceived(
            BluetoothLEAdvertisementWatcher const& _watcher,
            BluetoothLEAdvertisementReceivedEventArgs const& args)
        {
            using namespace winrt::Windows::Devices::Bluetooth::Advertisement;

            std::wstring name{};
            std::vector<winrt::guid> services{};
            std::vector<ManufacturerData> manufacturerData{};
            std::vector<AdvertisementData> advertisingData{};

            auto advertisement = args.Advertisement();
            if (advertisement)
            {
                name = advertisement.LocalName();
                auto serv = advertisement.ServiceUuids();
                if (serv)
                {
                    services.reserve(serv.Size());
                    for (const auto& uuid : serv)
                    {
                        services.push_back(uuid);
                    }
                }
                auto manufDataList = advertisement.ManufacturerData();
                if (manufDataList)
                {
                    manufacturerData.reserve(manufDataList.Size());
                    for (const auto& manuf : manufDataList)
                    {
                        auto size = manuf.Data().Length();
                        manufacturerData.emplace_back(ManufacturerData{ manuf.CompanyId(), manuf.Data() });
                    }
                }
                auto advDataList = advertisement.DataSections();
                if (advDataList)
                {
                    advertisingData.reserve(advDataList.Size());
                    for (const auto& adv : advDataList)
                    {
                        auto size = adv.Data().Length();
                        advertisingData.emplace_back(AdvertisementData{ adv.DataType(), adv.Data() });
                    }
                }
            }

            int txPower = 0;
            if (args.TransmitPowerLevelInDBm())
            {
                txPower = args.TransmitPowerLevelInDBm().GetInt16();
            }

            switch (args.AdvertisementType())
            {
            case BluetoothLEAdvertisementType::ConnectableUndirected:
            case BluetoothLEAdvertisementType::ConnectableDirected:
            case BluetoothLEAdvertisementType::ScannableUndirected:
            case BluetoothLEAdvertisementType::NonConnectableUndirected:
            {
                std::shared_ptr<DiscoveredPeripheral> peripheral{
                    new DiscoveredPeripheral(
                        args.Timestamp(),
                        args.BluetoothAddress(),
                        args.IsConnectable(),
                        args.RawSignalStrengthInDBm(),
                        txPower,
                        name,
                        services,
                        manufacturerData,
                        advertisingData) };

                _peripherals[peripheral->address()] = peripheral;
                notify(peripheral);
            }
            break;

            case BluetoothLEAdvertisementType::ScanResponse:
            {
                auto it = _peripherals.find(args.BluetoothAddress());
                if (it != _peripherals.end())
                {
                    std::shared_ptr<DiscoveredPeripheral> updatedPeripheral{
                        new DiscoveredPeripheral(
                            args.Timestamp(),
                            *it->second,
                            args.RawSignalStrengthInDBm(),
                            txPower,
                            name,
                            services,
                            manufacturerData,
                            advertisingData) };

                    _peripherals[updatedPeripheral->address()] = updatedPeripheral;
                    notify(updatedPeripheral);
                }
            }
            break;
            }
        }

        void notify(const std::shared_ptr<DiscoveredPeripheral>& peripheral)
        {
            if (_onPeripheralDiscovered &&
                (_requestedServices.empty() || Internal::isSubset(_requestedServices, peripheral->services())))
            {
                _onPeripheralDiscovered(peripheral);
            }
        }

        void onStopped(
            BluetoothLEAdvertisementWatcher const& _watcher,
            BluetoothLEAdvertisementWatcherStoppedEventArgs const& args)
        {
        }
    };
}
