$port = New-Object System.IO.Ports.SerialPort
$port.PortName = "COM7"
$port.BaudRate = 9600
$port.Parity = [System.IO.Ports.Parity]::None
$port.DataBits = 8
$port.StopBits = [System.IO.Ports.StopBits]::One
$port.ReadTimeout = 3000
$port.WriteTimeout = 1000

try {
    $port.Open()
    Write-Host "Port opened successfully"
    Start-Sleep -Milliseconds 500

    # Send move command
    Write-Host "Sending: @MOVEJ 100,100,100,0,0,0"
    $port.WriteLine("@MOVEJ 100,100,100,0,0,0")
    Start-Sleep -Milliseconds 3000

    # Read response
    while ($port.BytesToRead -gt 0) {
        $response = $port.ReadExisting()
        Write-Host "Response: $response"
    }

    # Wait for motion to complete
    Write-Host "Waiting for motion to complete..."
    Start-Sleep -Milliseconds 5000

    # Send home command
    Write-Host "Sending: @MOVEJ 0,0,0,0,0,0"
    $port.WriteLine("@MOVEJ 0,0,0,0,0,0")
    Start-Sleep -Milliseconds 3000

    # Read response
    while ($port.BytesToRead -gt 0) {
        $response = $port.ReadExisting()
        Write-Host "Response: $response"
    }

    Write-Host "Test complete"
}
catch {
    Write-Host "Error: $_"
}
finally {
    if ($port.IsOpen) {
        $port.Close()
        Write-Host "Port closed"
    }
}
