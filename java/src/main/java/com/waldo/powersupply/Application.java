package com.waldo.powersupply;

import com.fazecast.jSerialComm.SerialPort;
import com.waldo.libs.PicMessage;
import com.waldo.libs.SerialListener;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.ChartUtilities;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.axis.DateAxis;
import org.jfree.chart.axis.NumberAxis;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.renderer.xy.XYItemRenderer;
import org.jfree.chart.renderer.xy.XYLineAndShapeRenderer;
import org.jfree.data.time.Millisecond;
import org.jfree.data.time.TimeSeries;
import org.jfree.data.time.TimeSeriesCollection;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.concurrent.ThreadLocalRandom;

import static com.waldo.libs.PICUartManager.uartManager;

public class Application extends JPanel implements SerialListener {

    private TimeSeries voltage;
    private TimeSeries current;

    private JTextField voltageTf;
    private JButton sendBtn;

    private JLabel lastVoltageLbl;
    private JLabel lastCurrentLbl;

    public Application() {
        super(new BorderLayout());

        JPanel graphPanel = new JPanel();
        graphPanel.setLayout(new BoxLayout(graphPanel, BoxLayout.Y_AXIS));

        // Chart
        graphPanel.add(createVoltageChart());
        graphPanel.add(createCurrentChart());

        add(graphPanel, BorderLayout.CENTER);
        add(createSetPanel(), BorderLayout.EAST);

        uartManager().init("PIC", PicMessage.PicType.DsPic, this);
        uartManager().registerShutDownHook();
    }

    private ChartPanel createVoltageChart() {
        voltage = new TimeSeries("Vout");
        voltage.setMaximumItemCount(20);

        TimeSeriesCollection data = new TimeSeriesCollection();
        data.addSeries(voltage);

        DateAxis domain = new DateAxis("Time");
        NumberAxis range = new NumberAxis("Voltage");

        XYItemRenderer renderer = new XYLineAndShapeRenderer(true, false);
        renderer.setSeriesPaint(0, Color.RED);
        renderer.setSeriesStroke(0, new BasicStroke(3f, BasicStroke.CAP_BUTT, BasicStroke.JOIN_BEVEL));

        XYPlot plot = new XYPlot(data, domain, range, renderer);
        domain.setAutoRange(true);
        domain.setLowerMargin(0.0);
        domain.setUpperMargin(0.0);
        domain.setTickLabelsVisible(true);

        range.setStandardTickUnits(NumberAxis.createIntegerTickUnits());

        JFreeChart chart = new JFreeChart("Power supply", new Font("SansSerif", Font.BOLD, 24), plot, true);
        ChartUtilities.applyCurrentTheme(chart);

        ChartPanel chartPanel = new ChartPanel(chart, false);
        chartPanel.setBorder(BorderFactory.createCompoundBorder(
                BorderFactory.createEmptyBorder(5,5,5,5),
                BorderFactory.createLineBorder(Color.BLACK)
        ));

        return chartPanel;
    }

    private ChartPanel createCurrentChart() {
        current = new TimeSeries("Iout");
        current.setMaximumItemCount(20);

        TimeSeriesCollection data = new TimeSeriesCollection();
        data.addSeries(current);

        DateAxis domain = new DateAxis("Time");
        NumberAxis range = new NumberAxis("Current");

        XYItemRenderer renderer = new XYLineAndShapeRenderer(true, false);
        renderer.setSeriesPaint(0, Color.BLUE);
        renderer.setSeriesStroke(0, new BasicStroke(3f, BasicStroke.CAP_BUTT, BasicStroke.JOIN_BEVEL));

        XYPlot plot = new XYPlot(data, domain, range, renderer);
        domain.setAutoRange(true);
        domain.setLowerMargin(0.0);
        domain.setUpperMargin(0.0);
        domain.setTickLabelsVisible(true);

        range.setStandardTickUnits(NumberAxis.createIntegerTickUnits());

        JFreeChart chart = new JFreeChart("Power supply", new Font("SansSerif", Font.BOLD, 24), plot, true);
        ChartUtilities.applyCurrentTheme(chart);

        ChartPanel chartPanel = new ChartPanel(chart, false);
        chartPanel.setBorder(BorderFactory.createCompoundBorder(
                BorderFactory.createEmptyBorder(5,5,5,5),
                BorderFactory.createLineBorder(Color.BLACK)
        ));

        return chartPanel;
    }


    private JPanel createSetPanel() {
        JPanel setPanel = new JPanel();
        setPanel.setLayout(new BoxLayout(setPanel, BoxLayout.Y_AXIS));


        voltageTf = new JTextField();
        voltageTf.setPreferredSize(new Dimension(200, 30));
        sendBtn = new JButton("Send");
        sendBtn.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                String toSend = voltageTf.getText();
                if (toSend != null && !toSend.isEmpty()) {
                    if (toSend.matches("[0-9]+")) {
                        uartManager().write(PicMessage.createMessage("V", toSend));
                    }
                }
            }
        });

        lastCurrentLbl = new JLabel();
        lastVoltageLbl = new JLabel();


        JPanel voltagePnl = new JPanel(new BorderLayout());
        voltagePnl.add(voltageTf, BorderLayout.CENTER);
        voltagePnl.add(sendBtn, BorderLayout.EAST);

        JPanel valuesPnl = new JPanel();
        valuesPnl.setLayout(new BoxLayout(valuesPnl, BoxLayout.Y_AXIS));
        valuesPnl.add(lastVoltageLbl);
        valuesPnl.add(lastCurrentLbl);


        setPanel.add(voltagePnl);
        setPanel.add(valuesPnl);

        return setPanel;
    }

    private void addVoltageValue(double value) {
        double voltage = (value / 4095) * 2.048 * 5;
        lastVoltageLbl.setText(String.valueOf(voltage) + "V");
        this.voltage.add(new Millisecond(), voltage);
    }

    private void addCurrentValue(double current) {
        lastCurrentLbl.setText(String.valueOf(current) + "A");
        this.current.add(new Millisecond(), current);
    }

    @Override
    public void onInitSuccess(SerialPort serialPort) {
        System.out.println("Connected to: " + serialPort.getSystemPortName());
        uartManager().write(PicMessage.createMessage("S", "")); // Start
    }

    @Override
    public void onSerialError(String s) {
        System.err.println(s);
    }

    @Override
    public void onNewMessage(PicMessage picMessage) {
        switch (picMessage.getCommand()) {
            case "A0":
                addVoltageValue(Double.valueOf(picMessage.getMessage()));
                break;
            case "A1":
                addCurrentValue(Double.valueOf(picMessage.getMessage()));
                break;
            default:
                break;
        }
    }

    @Override
    public void onMessageSend(PicMessage picMessage) {

    }

    class DataGenerator extends Timer implements ActionListener {

        public DataGenerator(int interval) {
            super(interval, null);
            addActionListener(this);

        }

        public void actionPerformed(ActionEvent e) {
            double v = ThreadLocalRandom.current().nextDouble(0, 1000);
            double i = ThreadLocalRandom.current().nextDouble(0, 500);

            addVoltageValue(v);
            addCurrentValue(i);
        }
    }


}
