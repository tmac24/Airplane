//
//  ViewController.swift
//  Airplane
//
//  Created by sunt on 2026/3/4.
//

import UIKit

class ViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        
//        C++基础知识运行
//        let wrapper = CppWrapper()
//        let result = wrapper.add(withA: 3, b: 5)
//        print(result)
//        
//        let result1 = wrapper.add(withA1: 8, b: 10)
//        print(result1)
//        
//        wrapper.testC()

        
        
//        纯 C 接口桥接
//        let engine = engine_create()
//        engine_start(engine)
//        engine_destroy(engine)
        
        
        
//        //画三角形 ios直接画
//        let glView = GLView(frame: view.bounds)
//        view.addSubview(glView)
//        glView.startRender()
        
        
        
    
//        //包含 深度+模板缓冲对象 ios直接画
//        let glDepthView = GLDepthView(frame: view.bounds)
//        view.addSubview(glDepthView)
//        glDepthView.startRender()
        
        
        
        /**
         画三角形  GLAirplaneView（负责 iOS 环境） Triangle（负责 OpenGL 绘制）
         GLAirplaneView 负责：上下文、缓冲区和渲染循环
         context
         framebuffer
         render loop

         Triangle 负责：渲染逻辑
         shader
         VBO
         draw
         */
        let glAirView = GLAirplaneView(frame: view.bounds)
        view.addSubview(glAirView)
        glAirView.startRender()
        
        
        
        
        /**
         画三角形  GLAirplaneView（负责 iOS 环境） Triangle（负责 OpenGL 绘制）
         */
//        let airDepthView = GLAirplaneDepthView(frame: view.bounds)
//        view.addSubview(airDepthView)
//        airDepthView.startRender()
        
        
        

        
    }


}

